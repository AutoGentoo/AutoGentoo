//
// Created by atuser on 12/2/18.
//

#ifndef AUTOGENTOO_WORKER_H
#define AUTOGENTOO_WORKER_H

#include <stdio.h>
#include <pthread.h>

#define WORKER_SOCK "/tmp/autogentoo_worker.sock"
#define WORKER_SOCK_LCK "/tmp/autogentoo_worker.sock.lock"
#define WORKER_RECV_CHUNK_SIZE 32

#define WORKER_REQUEST_TEMPLATE "issia(s)"

typedef struct __WorkerHandler WorkerHandler;
typedef struct __Worker Worker;
typedef struct __WorkerRequest WorkerRequest;
typedef struct __WorkerWaitor WorkerWaitor;

#include "host.h"

struct __WorkerRequest {
	Worker* parent;
	
	Host* host;
	char* script;
	int chroot;
	
	int argument_n;
	char** arguments;
};

/* Worker never needs to do a search because its saved to a file */
struct __Worker {
	WorkerHandler* parent;
	Worker* next;
	Worker* back;
	
	WorkerRequest* request;
	pthread_mutex_t running;
	
	char* parent_directory;
	
	int exit_code;
	
	char* id;
	pthread_t pid;
	pid_t forked_pid;
};

struct __WorkerHandler {
	Worker* worker_head;
	pthread_mutex_t worker_mutex;
};

WorkerHandler* worker_handler_new();
void worker_start(Worker* worker);
void worker_free(Worker* worker);
void worker_handler_free(WorkerHandler* worker_handler);

char* worker_register();

char* worker_request(WorkerHandler* worker_handler, WorkerRequest* request);

#endif //AUTOGENTOO_WORKER_H
