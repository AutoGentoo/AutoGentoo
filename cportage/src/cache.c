//
// Created by atuser on 12/31/19.
//

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <openssl/md5.h>
#include "portage.h"
#include "ebuild/hash.h"
#include "cache.h"
#include "package.h"

int cache_verify(char* file, char* md5_expanded) {
	FILE* fp = fopen(file, "r");
	if (!fp)
		return 0;
	
	char old_md5[MD5_DIGEST_LENGTH * 2 + 1];
	old_md5[MD5_DIGEST_LENGTH * 2] = 0;
	fread(old_md5, MD5_DIGEST_LENGTH * 2, 1, fp);
	fclose(fp);
	
	return strcmp(old_md5, md5_expanded) == 0;
}

char* prv_cache_variable(Vector* dest, char* fmt, ...) {
	char* __dest_tm = NULL;
	
	va_list arg;
	va_start(arg, fmt);
	vasprintf(&__dest_tm, fmt, arg);
	va_end(arg);
	
	vector_add(dest, __dest_tm);
	return __dest_tm;
}

int cache_generate(Ebuild* target, int index) {
	if (!target)
		return 1;
	
	Vector* cache_to_free = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	char* pvr = NULL;
	
	if (target->revision > 0) {
		pvr = prv_cache_variable(cache_to_free, "PVR=%s-r%d", target->version->full_version, target->revision);
	}
	else {
		pvr = prv_cache_variable(cache_to_free, "PVR=%s", target->version->full_version);
	}
	
	printf("Cache %s/%s (thread %d)\n", target->category, target->pf, index);
	fflush(stdout);
	
	char* env[] = {
			"PORTAGE_BIN_PATH="PORTAGE_BIN_PATH,
			"PORTAGE_BUILDDIR="PORTAGE_BUILDDIR,
			"PORTAGE_TMP_DIR="PORTAGE_TMP_DIR,
			"WORKDIR="WORKDIR,
			"EBUILD_PHASE=setup",
			"EMERGE_FROM=ebuild",
			"T="T,
			"D="D,
			"HOME="HOME,
			"_IN_INSTALL_QA_CHECK=1",
			"PORTAGE_GRPNAME="PORTAGE_GROUPNAME,
			"EAPI="PORTAGE_EAPI,
			prv_cache_variable(cache_to_free, "EBUILD=%s", target->ebuild),
			prv_cache_variable(cache_to_free, "CATEGORY=%s", target->category),
			prv_cache_variable(cache_to_free, "P=%s-%s", target->name, target->version->full_version),
			prv_cache_variable(cache_to_free, "PN=%s", target->name),
			prv_cache_variable(cache_to_free, "PV=%s", target->version->full_version),
			prv_cache_variable(cache_to_free, "PORTAGE_ECLASS_LOCATIONS=%s", target->parent->parent->location),
			prv_cache_variable(cache_to_free, "CACHE_DIR="PORTAGE_CACHE"/%s", target->category),
			prv_cache_variable(cache_to_free, "CACHE_FILE=%s", target->cache_file),
			prv_cache_variable(cache_to_free, "MD5_HASH=%s", target->ebuild_md5),
			prv_cache_variable(cache_to_free, "PF=%s", target->pf),
			pvr,
			NULL
	};
	
	pid_t env_pid = fork();
	
	if (env_pid == 0) {
		remove(PORTAGE_BUILDDIR"/.setuped");
		char* args[] = {
				PORTAGE_BIN_PATH"/gencache.sh",
				NULL,
		};
		if (execve(PORTAGE_BIN_PATH"/gencache.sh", args, env) != 0) {
			plog_error("Failed to cache for %s/%s", target->category, target->name);
			errno = 0;
			plog_error("Failed to open %s", PORTAGE_BIN_PATH"/gencache.sh");
			
			exit(1);
		}
	}
	
	vector_foreach(cache_to_free, free);
	vector_free(cache_to_free);
	
	int env_ret = 0;
	waitpid(env_pid, &env_ret, 0);
	
	return env_ret;
}

CacheWorker* cache_worker_new(CacheHandler* parent) {
	CacheWorker* out = malloc(sizeof(CacheWorker));
	
	out->handler = parent;
	out->index = parent->number++;
	
	return out;
}

CacheHandler* cache_handler_new(int jobs) {
	CacheHandler* out = malloc(sizeof(CacheHandler));
	out->head = NULL;
	
	pthread_cond_init(&out->cond, NULL);
	pthread_mutex_init(&out->head_mutex, NULL);
	
	out->number = 0;
	out->jobs = jobs;
	out->pids = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	
	while (out->number < jobs) {
		CacheWorker* current_worker = cache_worker_new(out);
		if (pthread_create(&current_worker->pid, NULL, (void* (*)(void*)) cache_request_mainloop, current_worker)) {
			plog_error("Error creating thread");
			fflush(stdout);
			exit(1);
		}
		
		vector_add(out->pids, current_worker);
	}
	
	return out;
}

void cache_handler_request(CacheHandler* handler, Ebuild* target) {
	struct __CacheRequest* req = malloc(sizeof(struct __CacheRequest));
	req->target = target;
	
	pthread_mutex_lock(&handler->head_mutex);
	req->next = handler->head;
	handler->head = req;
	pthread_mutex_unlock(&handler->head_mutex);
}


void cache_request_mainloop(CacheWorker* worker) {
	pthread_mutex_lock(&worker->handler->head_mutex);
	pthread_cond_wait(&worker->handler->cond, &worker->handler->head_mutex);
	pthread_mutex_unlock(&worker->handler->head_mutex);
	
	while (1) {
		pthread_mutex_lock(&worker->handler->head_mutex);
		if (!worker->handler->head) {
			pthread_mutex_unlock(&worker->handler->head_mutex);
			break;
		}
		
		struct __CacheRequest* req = worker->handler->head;
		worker->handler->head = worker->handler->head->next;
		pthread_mutex_unlock(&worker->handler->head_mutex);
		
		int res = cache_generate(req->target, worker->index);
		fflush(stdout);
		if (res != 0) {
			errno = 0;
			plog_error("pthread failed -- %s", req->target->ebuild);
			exit(res);
		}
		
		free(req);
	}
}

void cache_handler_finish(CacheHandler* handler) {
	pthread_mutex_lock(&handler->head_mutex);
	pthread_cond_broadcast(&handler->cond);
	pthread_mutex_unlock(&handler->head_mutex);
	
	for (int i = 0; i < handler->jobs; i++)
		pthread_join(((CacheWorker*)vector_get(handler->pids, i))->pid, NULL);
	
	vector_foreach(handler->pids, free);
	free(handler->pids);
	free(handler);
}