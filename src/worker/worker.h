//
// Created by atuser on 12/2/18.
//

#ifndef AUTOGENTOO_WORKER_H
#define AUTOGENTOO_WORKER_H

#include <stdio.h>
#include <pthread.h>

typedef struct __WorkerHandler WorkerHandler;
typedef struct __Worker Worker;
typedef struct __WorkerRequest WorkerRequest;

struct __WorkerRequest {
	char* script;
	char* parent_directory;
	
	size_t argument_n;
	char** arguments;
	int chroot;
};

/* Worker never needs to do a search because its saved to a file */
struct __Worker {
	Worker* next;
	Worker* back;
	
	WorkerRequest* request;
	
	int id;
	pthread_t pid;
};

struct __WorkerHandler {
	Worker* worker_head;
	pthread_mutex_t worker_mutex;
	
	int sock;
	int highest_worker;
	char* config;
	
	int keep_alive;
};

WorkerHandler* worker_handler_new();
void worker_handler_start(WorkerHandler* worker_handler);
void worker_handler_handle(WorkerHandler* worker_handler, int accept);
Worker* worker_handler_job(WorkerHandler* worker_handler);
void worker_start(Worker* worker);

#endif //AUTOGENTOO_WORKER_H
