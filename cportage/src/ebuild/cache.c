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
#include <share.h>
#include "../portage.h"
#include "hash.h"
#include "cache.h"

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

int cache_generate(Ebuild* target, int thread_num) {
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
	
	printf("Cache %s/%s (thread %d)\n", target->category, target->pf, thread_num);
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
	out->request_queue = queue_new();
	
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
	pthread_mutex_lock(&handler->head_mutex);
	queue_add(handler->request_queue, target);
	pthread_mutex_unlock(&handler->head_mutex);
}


void cache_request_mainloop(CacheWorker* worker) {
	pthread_mutex_lock(&worker->handler->head_mutex);
	pthread_cond_wait(&worker->handler->cond, &worker->handler->head_mutex);
	pthread_mutex_unlock(&worker->handler->head_mutex);
	
	while (1) {
		pthread_mutex_lock(&worker->handler->head_mutex);
		if (!queue_peek(worker->handler->request_queue)) {
			pthread_mutex_unlock(&worker->handler->head_mutex);
			break;
		}
		
		Ebuild* target = queue_pop(worker->handler->request_queue);
		pthread_mutex_unlock(&worker->handler->head_mutex);
		
		int res = cache_generate(target, worker->index);
		fflush(stdout);
		if (res != 0) {
			errno = 0;
			plog_error("pthread failed -- %s", target->ebuild);
			exit(res);
		}
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

void ebuild_metadata(Ebuild* ebuild) {
	if (ebuild->metadata_init)
		return;
	
	FILE* fp = fopen(ebuild->cache_file, "r");
	if (!fp) {
		plog_error("Failed to open %s", ebuild->cache_file);
		return;
	}
	
	for (int i = 0; i < ARCH_END; i++)
		ebuild->keywords[i] = KEYWORD_NONE;
	
	size_t name_size_n;
	char* name = NULL;
	
	size_t value_size_n;
	char* value = NULL;
	
	char* line = NULL;
	size_t line_size = 0;
	ssize_t n = getline(&line, &line_size, fp);
	line[n - 1] = 0;
	if (strcmp(line, ebuild->ebuild_md5) != 0) {
		fclose(fp);
		int cache_res = cache_generate(ebuild, 0);
		if (cache_res != 0)
			portage_die("Failed to generate cache for %s/%s", ebuild->category, ebuild->pf);
		
		fp = fopen(ebuild->cache_file, "r");
		if (!fp) {
			free(line);
			portage_die("Failed to open %s", ebuild->cache_file);
			return;
		}
		
		getline(&line, &line_size, fp);
		if (strcmp(line, ebuild->ebuild_md5) != 0) {
			fclose(fp);
			free(line);
			portage_die("Failed to generate valid cache file for %s/%s", ebuild->category, ebuild->pf);
		}
	}
	
	free(line);
	
	while(!feof(fp)) {
		size_t name_size = getdelim(&name, &name_size_n, '=', fp);
		size_t value_size = getdelim(&value, &value_size_n, '\n', fp);
		
		if (!name || !value)
			break;
		
		name[name_size - 1] = 0;
		value[value_size - 1] = 0;
		
		if (strcmp(name, "DEPEND") == 0) {
			if (ebuild->depend)
				portage_die("DEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->depend = depend_parse(value);
		}
		else if (strcmp(name, "RDEPEND") == 0) {
			if (ebuild->rdepend)
				portage_die("RDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->rdepend = depend_parse(value);
		}
		else if (strcmp(name, "PDEPEND") == 0) {
			if (ebuild->pdepend)
				portage_die("PDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->pdepend = depend_parse(value);
		}
		else if (strcmp(name, "BDEPEND") == 0) {
			if (ebuild->bdepend)
				portage_die("BDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->bdepend = depend_parse(value);
		}
		else if (strcmp(name, "SLOT") == 0) {
			char* tok = strtok(value, "/");
			ebuild->slot = strdup(tok);
			
			tok = strtok(NULL, "/");
			if (tok)
				ebuild->sub_slot = strdup(tok);
		}
		else if (strcmp(name, "REQUIRED_USE") == 0)
			ebuild->required_use = required_use_parse(value);
		else if (strcmp(name, "KEYWORDS") == 0)
			keyword_parse(ebuild->keywords, value);
		else if (strcmp(name, "IUSE") == 0)
			ebuild->use = use_iuse_parse(ebuild->parent->parent->parent, value);
	}
	
	free(value);
	free(name);
	
	ebuild->metadata_init = 1;
	fclose(fp);
}