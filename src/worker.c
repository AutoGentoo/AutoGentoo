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

WorkerHandler* worker_handler_new() {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	out->worker_head = NULL;
	out->current_request = NULL;
	
	pthread_mutex_init(&out->worker_mutex, NULL);
	pthread_mutex_init(&out->worker_wait_mutex, NULL);
	pthread_cond_init(&out->worker_wait, NULL);
	
	return out;
}

void worker_handler_start(WorkerHandler* worker_handler) {
	while(worker_handler->keep_alive) {
		pthread_mutex_lock(&worker_handler->worker_wait_mutex);
		pthread_cond_wait(&worker_handler->worker_wait, &worker_handler->worker_wait_mutex);
		
		pthread_mutex_lock(&worker_handler->worker_mutex);
		
		Worker* worker = worker_handler->current_request;
		worker_handler->current_request = NULL;
		pthread_create(&worker->pid, NULL, (void* (*) (void*))worker_start, worker);
	}
}

void worker_start(Worker* worker) {
	// Worker mutex is locked
	
	worker->next = worker->parent->worker_head;
	worker->back = NULL;
	worker->parent->worker_head = worker;
	
	if (worker->next)
		worker->next->back = worker;
	
	pthread_mutex_unlock(&worker->parent->worker_mutex);
	
	if (worker->request->chroot)
		chroot(worker->request->parent_directory);
	else
		chdir(worker->request->parent_directory);
	
	int self_pid = fork();
	
	if (self_pid == 0) {
		char* filename;
		asprintf(&filename, "log/%s.log", worker->id);
		FILE* log = fopen(filename, "w+");
		free(filename);
		
		int fdlog = fileno(log);
		dup2(fdlog, 1);
		dup2(fdlog, 2);
		fclose(log);
		
		execv(worker->request->script, worker->request->arguments);
		lerror("error running %s", worker->request->script);
		exit(-1);
	}
	
	waitpid (self_pid, &worker->exit_code, 0);
	linfo("worker (%s) exited with %d", worker->id, worker->exit_code);
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
	
	linfo("Registered worker with id: %s", out);
	return out;
}

char* worker_request(WorkerHandler* worker_handler, WorkerRequest* request) {
	pthread_mutex_lock(&worker_handler->worker_mutex);
	
	Worker* worker = malloc(sizeof(Worker));
	worker->parent = worker_handler;
	worker->request = request;
	worker_handler->current_request = worker;
	worker->id = worker_register();
	worker->request->parent = worker;
	
	pthread_mutex_unlock(&worker_handler->worker_mutex);
	pthread_cond_signal(&worker_handler->worker_wait);
	
	return worker->id;
}
