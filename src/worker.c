//
// Created by atuser on 11/30/18.
//

#define _GNU_SOURCE

#include "autogentoo/worker.h"
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

WorkerHandler* worker_handler_new() {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	pthread_mutex_init(&out->sig_lck, NULL);
	pthread_mutex_init(&out->write_lck, NULL);
	pthread_mutex_init(&out->read_lck, NULL);
	pthread_mutex_init(&out->lck, NULL);
	pthread_mutex_init(&out->request_lck, NULL);
	
	pthread_cond_init(&out->sig, NULL);
	
	out->read_fifo = -1;
	out->write_fifo = -1;
	
	out->request = NULL;
	
	out->keep_alive = 1;
	
	return out;
}

int worker_handler_start(WorkerHandler* wh) {
	if (mkfifo(WORKER_FIFO_REQUEST, 0600) == -1) {
		if (errno != EEXIST) {
			lerror("Failed to init req_fifo: %s", strerror(errno));
			return 1;
		}
	}
	
	if (mkfifo(WORKER_FIFO_RESPONSE, 0600) == -1) {
		if (errno != EEXIST) {
			lerror("Failed to init res_fifo: %s", strerror(errno));
			return 1;
		}
	}
	
	wh->write_fifo = open(WORKER_FIFO_REQUEST, O_WRONLY);
	wh->read_fifo = open(WORKER_FIFO_RESPONSE, O_RDONLY);
	
	pthread_create(&wh->pid, NULL, (void* (*) (void*))worker_handler_loop, wh);
	
	return 0;
}

void worker_handler_loop(WorkerHandler* wh) {
	linfo("Started worker handler");
	wh->request = NULL;
	
	while (wh->keep_alive) {
		if (!wh->request) {
			/* We've handled all current requests
			 * Wait for another one
			 */
			
			pthread_mutex_lock(&wh->sig_lck);
			pthread_mutex_lock(&wh->request_lck); /* Wait for request process to exit first */
			pthread_cond_wait(&wh->sig, &wh->sig_lck);
			
			continue; /* Check if there is a request, could be false signal */
		}
		
		pthread_mutex_lock(&wh->write_lck);
		pthread_mutex_unlock(&wh->sig_lck);
		
		pthread_mutex_lock(&wh->lck);
		pthread_mutex_unlock(&wh->request_lck);
		
		WorkerRequest* req = wh->request;
		char* command_id = worker_register(req->command_name);
		wh->request = NULL;
		
		pthread_mutex_unlock(&wh->lck);
		
		/* Write the request to the write_fifo */
		write(wh->write_fifo, command_id, strlen(command_id) + 1);
		write(wh->write_fifo, req->template, strlen(req->template) + 1);
		write(wh->write_fifo, req->bytes, req->n);
		
		free(command_id);
		
		pthread_mutex_unlock(&wh->write_lck);
	}
}

int worker_handler_request(WorkerHandler* wh, WorkerRequest* request, char** command_id) {
	pthread_mutex_lock(&wh->lck);
	wh->request = request;
	pthread_mutex_unlock(&wh->lck);
	
	pthread_cond_signal(&wh->sig);
	pthread_mutex_lock(&wh->request_lck); /* Wait for the write lock to activate */
	pthread_mutex_lock(&wh->write_lck); /* Wait for request to be sent */
	
	/* Release the locks */
	pthread_mutex_unlock(&wh->write_lck);
	pthread_mutex_unlock(&wh->request_lck);
	
	/* Read from the worker process */
	pthread_mutex_lock(&wh->read_lck);
	int res = 0;
	read(wh->read_fifo, &res, sizeof(int));
	
	int read_len = 0;
	read(wh->read_fifo, &read_len, sizeof(int));
	
	*command_id = malloc(read_len);
	read(wh->read_fifo, *command_id, read_len);
	
	pthread_mutex_unlock(&wh->read_lck);
	
	return res;
}

int prv_random_string(char* out, size_t len);

char* worker_register(char* command_name) {
	time_t rawtime;
	struct tm *info;
	
	size_t len = strlen(command_name);
	
	char* out = malloc(24 + len + 1);
	
	time( &rawtime );
	info = localtime( &rawtime );
	
	strftime(out, 32, "%Y-%m-%d-%H-%M-%S-", info);
	prv_random_string(out + 20, 4);
	strcpy(out + 24, command_name);
	out[24 + len] = 0;
	
	return out;
}

void worker_handler_free(WorkerHandler* wh) {
	linfo("Exiting WorkerHandler with worker_handler_free()");
	
	wh->keep_alive = 0;
	
	pthread_cond_signal(&wh->sig);
	pthread_join(wh->pid, NULL);
	
	pthread_mutex_destroy(&wh->sig_lck);
	pthread_mutex_destroy(&wh->write_lck);
	pthread_mutex_destroy(&wh->read_lck);
	pthread_mutex_destroy(&wh->lck);
	pthread_mutex_destroy(&wh->request_lck);
	
	pthread_cond_destroy(&wh->sig);
	
	close(wh->read_fifo);
	close(wh->write_fifo);
	
	free(wh);
}
