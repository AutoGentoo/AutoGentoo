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

struct __WorkerRequest {
	Worker* parent;
	
	char* script;
	char* parent_directory;
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
	
	int exit_code;
	
	char* id;
	pthread_t pid;
};

struct __WorkerHandler {
	Worker* worker_head;
	pthread_mutex_t worker_mutex;
	
	pthread_mutex_t worker_wait_mutex;
	pthread_cond_t worker_wait;
	
	Worker* current_request; // stack
	
	pthread_t handler_pid;
	int keep_alive;
};

WorkerHandler* worker_handler_new();
void worker_handler_start(WorkerHandler* worker_handler);
void worker_start(Worker* worker);
void worker_free(Worker* worker);

char* worker_register();

char* worker_request(WorkerHandler* worker_handler, WorkerRequest* request);

#endif //AUTOGENTOO_WORKER_H
