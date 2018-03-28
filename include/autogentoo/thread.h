//
// Created by atuser on 1/14/18.
//

#ifndef AUTOGENTOO_THREAD_H
#define AUTOGENTOO_THREAD_H

#include <stdio.h>
#include <stdio.h>
#include <pthread.h>

typedef struct _ThreadHandler ThreadHandler;
typedef struct _ThreadRegister ThreadRegister;

struct _ThreadHandler {
	pthread_t to_join;
	size_t conn_max;
	ThreadRegister* threads;
};

#include "server.h"

struct _ThreadRegister {
	pthread_t thread;
	Connection* conn;
};

Connection* thread_get_conn (ThreadHandler* handler, pthread_t thread);

ThreadHandler* thread_handler_new(size_t conn_max);

void thread_join(ThreadHandler* handler, pthread_t thread);

void thread_register(ThreadHandler* handler, pthread_t thread, Connection* conn);

void thread_handler_join_all(ThreadHandler* handler);

void thread_handler_free(ThreadHandler* handler);

#endif //AUTOGENTOO_THREAD_H
