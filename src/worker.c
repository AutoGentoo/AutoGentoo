//
// Created by atuser on 11/30/18.
//

#define _GNU_SOURCE

#include "autogentoo/worker.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <autogentoo/hacksaw/tools.h>
#include <autogentoo/request_structure.h>
#include <fcntl.h>
#include <wait.h>
#include <autogentoo/api/ssl_wrap.h>
#include <sys/file.h>
#include <errno.h>

WorkerHandler* worker_handler_new() {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	out->worker_head = NULL;
	pthread_mutex_init(&out->worker_mutex, NULL);
	
	return out;
}

void worker_start(Worker* worker) {
	pid_t self_pid = fork();
	
	if (self_pid == -1)
		lerror("Failed to create child process");
	
	if (self_pid == 0) {
		char* filename;
		linfo("Registered worker with id: %s", worker->id);
		asprintf(&filename, "log/%s-%s.log", worker->request->host->id, worker->id);
		int log = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWUSR);
		free(filename);
		
		if (log == -1) {
			lerror("Failed to open log file %s", filename);
			exit(1);
		}
		/*
		if (worker->request->chroot) {
			if (chroot(worker->parent_directory) == -1) {
				free(parent_directory);
				exit(2);
			}
		}
		else {
			if (chdir(worker->parent_directory) == -1) {
				free(parent_directory);
				exit(3);
			}
		}*/
		
		dup2(log, 1);
		dup2(log, 2);
		close(log);
		
		int ret = execv(worker->request->script, worker->request->arguments);
		lerror("error running %s", worker->request->script);
		exit(ret);
	}
	
	if (self_pid != -1)
		waitpid (self_pid, &worker->exit_code, 0);
	
	linfo("worker (%s) exited with %d", worker->id, worker->exit_code);
	pthread_mutex_unlock(&worker->running);
	
	return worker_free(worker);
}

void worker_free(Worker* worker) {
	pthread_mutex_lock(&worker->parent->worker_mutex);
	
	if (worker->parent->worker_head == worker)
		worker->parent->worker_head = worker->next;
	
	if (worker->back)
		worker->back->next = worker->next;
	if (worker->next)
		worker->next->back = worker->back;
	
	pthread_mutex_unlock(&worker->parent->worker_mutex);
	
	free(worker->id);
	free(worker);
}

int prv_random_string(char* out, size_t len);

char* worker_register() {
	time_t rawtime;
	struct tm *info;
	char* out = malloc(32);
	
	time( &rawtime );
	info = localtime( &rawtime );
	
	strftime(out, 32, "%Y-%m-%d-%H-%M-%S-", info);
	prv_random_string(out + 20, 4);
	
	return out;
}

char* worker_request(WorkerHandler* worker_handler, WorkerRequest* request) {
	pthread_mutex_lock(&worker_handler->worker_mutex);
	
	Worker* worker = malloc(sizeof(Worker));
	worker->parent = worker_handler;
	worker->request = request;
	worker->id = worker_register();
	worker->request->parent = worker;
	
	pthread_mutex_init(&worker->running, NULL);
	worker->next = worker->parent->worker_head;
	worker->back = NULL;
	worker->parent->worker_head = worker;
	
	if (worker->next)
		worker->next->back = worker;
	
	pthread_mutex_unlock(&worker->parent->worker_mutex);
	
	char* parent_directory = host_path(worker->request->host, "");
	
	struct stat st;
	if (stat(parent_directory, &st) == -1) {
		lerror("Failed to start %s", worker->id);
		lerror("Error [%d] %s", errno, strerror(errno));
		
		worker_free(worker);
		
		return NULL;
	}
	if (stat("log", &st) == -1) {
		lerror("Failed to start %s", worker->id);
		lerror("[Log directory]");
		lerror("Error [%d] %s", errno, strerror(errno));
		
		worker_free(worker);
		
		return NULL;
	}
	
	pthread_mutex_lock(&worker->running);
	
	pthread_create(&worker->pid, NULL, (void* (*) (void*))worker_start, worker);
	
	return worker->id;
}
