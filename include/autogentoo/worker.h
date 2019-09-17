//
// Created by atuser on 12/2/18.
//

#ifndef AUTOGENTOO_WORKER_H
#define AUTOGENTOO_WORKER_H

#include <stdio.h>
#include <pthread.h>

#ifndef AUTOGENTOO_WORKER_DIR
#define AUTOGENTOO_WORKER_DIR "/usr/share/autogentoo/"
#endif

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
	Server* parent;
	pthread_t pid;
	pid_t worker_pid;
	
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

WorkerHandler* worker_handler_new(Server* parent);
int worker_handler_start(WorkerHandler* wh);

/**
 * Main entry point into the python worker environment
 * This will send interupt to wh->sig and pull the request
 * across the pipe to the worker side
 *
 * @param wh the handle with pipe/locks initialized
 * @param request request to send over
 * @param job_id pointer to NULL, after this returns, the job_name will be stored (needs to be freed)
 * @return integer representing handling status of request. 0 is good, anything else is bad
 */
int worker_handler_request(WorkerHandler* wh, WorkerRequest* request, char** job_id);

void worker_handler_loop(WorkerHandler* wh);
char* worker_register(char* host_id, char* command_name);
void worker_handler_free(WorkerHandler* wh);

#endif //AUTOGENTOO_WORKER_H
