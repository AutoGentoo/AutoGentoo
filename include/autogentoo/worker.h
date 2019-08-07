//
// Created by atuser on 12/2/18.
//

#ifndef AUTOGENTOO_WORKER_H
#define AUTOGENTOO_WORKER_H

#include <stdio.h>
#include <pthread.h>

#define WORKER_FIFO_REQUEST "/tmp/autogentoo_worker.req"
#define WORKER_FIFO_RESPONSE "/tmp/autogentoo_worker.res"

typedef struct __WorkerHandler WorkerHandler;
typedef struct __WorkerRequest WorkerRequest;

#include "host.h"

struct __WorkerRequest {
	char* command_name;
	char* host_id;
	
	int n;
	char** args;
};

struct __WorkerHandler {
	pthread_t pid;
	
	pthread_mutex_t sig_lck;
	pthread_cond_t sig;
	
	int read_fifo;
	int write_fifo;
	
	pthread_mutex_t write_lck;
	pthread_mutex_t read_lck;
	
	pthread_mutex_t request_lck;
	pthread_mutex_t lck;
	
	WorkerRequest* request;
	
	int keep_alive;
};

WorkerHandler* worker_handler_new();
int worker_handler_start(WorkerHandler* wh);
int worker_handler_request(WorkerHandler* wh, WorkerRequest* request, char** command_id);
void worker_handler_loop(WorkerHandler* wh);
char* worker_register(char* command_name);
void worker_handler_free(WorkerHandler* wh);

#endif //AUTOGENTOO_WORKER_H
