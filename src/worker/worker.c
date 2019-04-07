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
#include <fcntl.h>
#include <wait.h>
#include <autogentoo/api/ssl_wrap.h>

#define WORKER_TCP "worker.tcp"
#define WORKER_CONFIG ".worker.config"
#define WORKER_RECV_CHUNK_SIZE 32

WorkerHandler* worker_handler_new() {
	WorkerHandler* out = malloc(sizeof(WorkerHandler));
	
	out->config = strdup(WORKER_CONFIG);
	out->worker_head = NULL;
	out->keep_alive = 1;
	
	pthread_mutex_init(&out->worker_mutex, NULL);
	
	return out;
}

void worker_handler_start(WorkerHandler* worker_handler) {
	worker_handler->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (access(WORKER_TCP, F_OK) == 0)
		remove(WORKER_TCP);
	
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
	
	linfo("Started worker handler at %s", WORKER_TCP);
	while(worker_handler->keep_alive) {
		int accepted = accept(worker_handler->sock, (struct sockaddr*) &addr, &addrlen);
		Worker* worker = malloc(sizeof(Worker));
		worker->parent = worker_handler;
		worker->accept = accepted;
		
		pthread_create(&worker->pid, NULL, (void* (*) (void*))worker_start, worker);
	}
}

void worker_start(Worker* worker) {
	void* request_data;
	worker->id = worker_register();
	size_t request_size = socket_read(worker->accept, &request_data, 1);
	write(worker->accept, worker->id, 25);
	close(worker->accept);
	
	worker->request_size = request_size;
	worker->request = malloc(sizeof(WorkerRequest));
	parse_request_structure ((RequestData*)worker->request, WORKER_REQUEST_TEMPLATE, request_data, request_data + request_size);
	
	worker->accept = 0;
	
	pthread_mutex_lock(&worker->parent->worker_mutex);
	
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
		asprintf(&filename, "%s.log", worker->id);
		FILE* log = fopen(filename, "w+");
		free(filename);
		
		int fdlog = fileno(log);
		dup2(fdlog, 1);
		dup2(fdlog, 2);
		fclose(log);
		
		char* script;
		asprintf(&script, "scripts/%s", worker->request->script);
		execv(script, worker->request->arguments);
	}
	
	int ret;
	waitpid (self_pid, &ret, 0);
	
	worker_free(worker);
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
	free_request_structure((RequestData*)worker->request, WORKER_REQUEST_TEMPLATE, worker->request + worker->request_size);
	
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