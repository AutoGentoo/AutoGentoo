//
// Created by atuser on 12/2/18.
//

#ifndef AUTOGENTOO_WORKER_H
#define AUTOGENTOO_WORKER_H

#include <stdio.h>
#include <pthread.h>

#define WORKER_REQUEST_TEMPLATE "ssia(s)"

typedef struct __WorkerHandler WorkerHandler;
typedef struct __Worker Worker;
typedef struct __WorkerRequest WorkerRequest;

struct __WorkerRequest {
	char* script;
	char* parent_directory;
	int chroot;
	
	size_t argument_n;
	char** arguments;
};

/* Worker never needs to do a search because its saved to a file */
struct __Worker {
	int accept; /* 0 during script exec */
	WorkerHandler* parent;
	Worker* next;
	Worker* back;
	
	size_t request_size;
	WorkerRequest* request;
	
	char* id;
	pthread_t pid;
};

struct __WorkerHandler {
	Worker* worker_head;
	pthread_mutex_t worker_mutex;
	
	int sock;
	char* config;
	
	int keep_alive;
};

WorkerHandler* worker_handler_new();
void worker_handler_start(WorkerHandler* worker_handler);
void worker_handler_handle(WorkerHandler* worker_handler, int accept);
Worker* worker_handler_job(WorkerHandler* worker_handler);
void worker_start(Worker* worker);
void worker_free(Worker* worker);

char* worker_register();

#endif //AUTOGENTOO_WORKER_H
