//
// Created by atuser on 11/30/18.
//

#define _GNU_SOURCE

#include "worker.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <autogentoo/hacksaw/tools.h>
#include <autogentoo/request_structure.h>
#include <bits/fcntl-linux.h>

#define WORKER_TCP "worker.tcp"
#define WORKER_CONFIG ".worker.config"
#define WORKER_RECV_CHUNK_SIZE 32

WorkerHandler* worker_handler_new() {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	out->config = strdup(WORKER_CONFIG);
	out->worker_head = NULL;
	out->highest_worker = 1;
	out->keep_alive = 1;
	
	pthread_mutex_init(&out->worker_mutex, NULL);
	
	return out;
}

void worker_handler_start(WorkerHandler* worker_handler) {
	worker_handler->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	
	struct sockaddr_un addr;
	socklen_t addrlen = sizeof(addr);
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, WORKER_TCP, sizeof(addr.sun_path)-1);
	if (bind(worker_handler->sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		lerror("Failed to bind socket to %s", WORKER_TCP);
		close (worker_handler->sock);
		return;
	}
	
	if (listen(worker_handler->sock, 32) == -1) {
		lerror("Error listen()");
		close(worker_handler->sock);
		return;
	}
	
	while(worker_handler->keep_alive) {
		int accepted = accept(worker_handler->sock, (struct sockaddr*) &addr, &addrlen);
		worker_handler_handle(worker_handler, accepted);
	}
}

void worker_handler_handle(WorkerHandler* worker_handler, int accept) {
	int request_size;
	read(worker_handler->sock, &request_size, sizeof(int));
	request_size = ntohl(request_size);
	
	void* request_data = malloc(request_size);
	int chunk_num = request_size / WORKER_RECV_CHUNK_SIZE;
	
	for (int i = 0; i < chunk_num; i++)
		read(accept, request_data + WORKER_RECV_CHUNK_SIZE * i, WORKER_RECV_CHUNK_SIZE);
	read(accept, request_data + WORKER_RECV_CHUNK_SIZE * chunk_num, request_size % WORKER_RECV_CHUNK_SIZE);
	
	Worker* worker = worker_handler_job(worker_handler);
	parse_request_structure ((RequestData*)worker->request, "sssa(s)", request_data, request_data + request_size);
	
	int worker_id = htonl(worker->id);
	write(accept, &worker_id, sizeof(int));
	close(accept);
	
	pthread_create(&worker->pid, NULL, (void* (*) (void*))worker_start, worker);
}

Worker* worker_handler_job(WorkerHandler* worker_handler) {
	pthread_mutex_lock(&worker_handler->worker_mutex);
	
	Worker* new_worker = malloc(sizeof(Worker));
	new_worker->next = worker_handler->worker_head;
	new_worker->back = NULL;
	new_worker->id = worker_handler->highest_worker++;
	worker_handler->worker_head = new_worker;
	
	if (new_worker->next)
		new_worker->next->back = new_worker;
	
	pthread_mutex_unlock(&worker_handler->worker_mutex);
	return new_worker;
}

void worker_start(Worker* worker) {
	char* filename;
	asprintf(&filename, "%d.log", worker->id);
	
	FILE* log = fopen(filename, "w+");
	
	if (worker->request->chroot)
		chroot(worker->request->parent_directory);
	else
		chdir(worker->request->parent_directory);
	
	int self_pid = fork();
	
	if (self_pid == 0) {
		int fdlog = fileno(log);
		dup2(fdlog, 1);
		dup2(fdlog, 2);
		fclose(log);
		
		char* script;
		asprintf(&script, "scripts/%s", worker->request->script);
		execv(script, worker->request->arguments);
	}
	
	
}