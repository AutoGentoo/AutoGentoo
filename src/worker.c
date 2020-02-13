//
// Created by atuser on 11/30/18.
//

#define _GNU_SOURCE

#include "autogentoo/worker.h"
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>
#include <wait.h>

WorkerHandler* worker_handler_new(Server* parent) {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	out->parent = parent;
	
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
	pthread_mutex_lock(&wh->parent->worker_ready);
	
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
	
	/* Start the worker daemon */
	
	char pid_str[16];
	sprintf(pid_str, "%d", getpid());
	
	wh->worker_pid = fork();
	if (wh->worker_pid == 0) {
		char* path = AUTOGENTOO_WORKER_DIR "/worker.py";
		char* const argv[] = {path, wh->parent->location, pid_str, NULL};
		
		int res = execv(path, argv);
		lerror("Failed to start worker");
		lerror("Error [%d] %s", res, strerror(res));
		exit(1);
	}
	else {
		/* Open as read/write so open doesn't hang */
		/* Opening a O_WRONLY with O_NONBLOCK causes ENXIO */
		wh->write_fifo = open(WORKER_FIFO_REQUEST, O_WRONLY);
		wh->read_fifo = open(WORKER_FIFO_RESPONSE, O_RDONLY);
	}
	
	pthread_create(&wh->pid, NULL, (void* (*) (void*))worker_handler_loop, wh);
	pthread_mutex_lock(&wh->parent->worker_ready);
	pthread_mutex_unlock(&wh->parent->worker_ready);
	
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
		char* command_id = worker_register(req->host_id, req->command_name);
		wh->request = NULL;
		
		pthread_mutex_unlock(&wh->lck);
		
		/* Write the request to the write_fifo */
		int command = WORKER_JOB;
		write(wh->write_fifo, &command, sizeof(int));
		
		int len = (int)strlen(command_id);
		write(wh->write_fifo, &len, sizeof(int));
		write(wh->write_fifo, command_id, len);
		
		len = (int)strlen(req->command_name);
		write(wh->write_fifo, &len, sizeof(int));
		write(wh->write_fifo, req->command_name, len);
		
		if (!req->host_id) {
			len = 0;
			write(wh->write_fifo, &len, sizeof(int));
		}
		else {
			len = (int)strlen(req->host_id);
			write(wh->write_fifo, &len, sizeof(int));
			write(wh->write_fifo, req->host_id, len);
		}
		
		len = req->n;
		write(wh->write_fifo, &len, sizeof(int));
		
		for (int i = 0; i < req->n; i++) {
			len = (int)strlen(req->args[i]);
			write(wh->write_fifo, &len, sizeof(int));
			write(wh->write_fifo, req->args[i], len);
		}
		
		free(command_id);
		
		pthread_mutex_unlock(&wh->write_lck);
	}
	
	int exit_cmd = WORKER_EXIT;
	write(wh->write_fifo, &exit_cmd, sizeof(int));
	
	/* Wait for the worker to exit */
	wait(NULL);
}

int worker_handler_request(WorkerHandler* wh, WorkerRequest* request, char** job_id) {
	pthread_mutex_lock(&wh->lck);
	wh->request = request;
	pthread_mutex_unlock(&wh->lck);

	/* Signal the C worker loop to send our request over */
	pthread_cond_signal(&wh->sig);
	pthread_mutex_lock(&wh->request_lck); /* Wait for the write lock to activate */
	pthread_mutex_lock(&wh->write_lck); /* Wait for request to be sent */
	
	/* Release the locks */
	pthread_mutex_unlock(&wh->write_lck);
	pthread_mutex_unlock(&wh->request_lck);
	
	/* Read from the worker process */
	pthread_mutex_lock(&wh->read_lck);
	int read_len = 0;
	read(wh->read_fifo, &read_len, sizeof(int));
	
	*job_id = malloc(read_len);
	read(wh->read_fifo, *job_id, read_len);
	
	int res = 0;
	read(wh->read_fifo, &res, sizeof(int));
	
	printf("WORKER RES %d\n", res);
	
	pthread_mutex_unlock(&wh->read_lck);
	
	return res;
}

int prv_random_string(char* out, size_t len);

char* worker_register(char* host_id, char* command_name) {
	time_t rawtime;
	struct tm *info;
	
	size_t len = strlen(command_name);
	
	char* out = malloc(24 + len + 1 + 16);
	
	time( &rawtime );
	info = localtime( &rawtime );
	
	size_t host_id_len = strlen(host_id);
	
	strcpy(out, host_id);
	
	strftime(out + host_id_len, 32, "%Y-%m-%d-%H-%M-%S-", info);
	prv_random_string(out + host_id_len + 20, 4);
	strcpy(out + host_id_len + 24, command_name);
	out[host_id_len + 24 + len] = 0;
	
	return out;
}

void worker_handler_free(WorkerHandler* wh) {
	wh->keep_alive = 0;
	
	/* Tell the worker thread to exit */
	pthread_cond_signal(&wh->sig);
	pthread_join(wh->pid, NULL);
	
	/* Free the mutexes */
	pthread_mutex_destroy(&wh->sig_lck);
	pthread_mutex_destroy(&wh->write_lck);
	pthread_mutex_destroy(&wh->read_lck);
	pthread_mutex_destroy(&wh->lck);
	pthread_mutex_destroy(&wh->request_lck);
	
	pthread_cond_destroy(&wh->sig);
	
	close(wh->read_fifo);
	close(wh->write_fifo);
	
	free(wh);
	
	linfo("Exited WorkerHandler with worker_handler_free()");
}

void worker_lock(int fd) {
	flock(fd, LOCK_EX);
}

void worker_unlock(int fd) {
	flock(fd, LOCK_UN);
}

int worker_job(Host* host, char* command, char** job_name, int argc, ...) {
	WorkerRequest worker_req;
	worker_req.command_name = "stage3";
	worker_req.host_id = host->id;
	
	worker_req.n = argc;
	worker_req.args = malloc(sizeof(char*) * argc);
	
	va_list args;
	va_start(args, argc);
	
	for (int i = 0; i < argc; i++) {
		worker_req.args[i] = va_arg(args, char*);
	}
	
	va_end(args);
	
	return worker_handler_request(host->parent->job_handler, &worker_req, job_name);
}