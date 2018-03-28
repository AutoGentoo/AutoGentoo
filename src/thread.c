//
// Created by atuser on 1/14/18.
//

#include <autogentoo/thread.h>
#include <stdlib.h>
#include <memory.h>
#include <autogentoo/hacksaw/tools.h>

Connection* thread_get_conn (ThreadHandler* handler, pthread_t thread) {
	int i;
	for (i = 0; i != handler->conn_max; i++) {
		ThreadRegister k = handler->threads[i];
		if (k.thread == thread)
			return k.conn;
	}
	return NULL;
}

ThreadHandler* thread_handler_new(size_t conn_max) {
	ThreadHandler* out = malloc(sizeof(ThreadHandler));
	out->threads = calloc(conn_max, sizeof(pthread_t));
	out->conn_max = conn_max;
	
	return out;
}

void thread_join(ThreadHandler* handler, pthread_t thread) {
	pthread_join(thread, NULL);
	int i;
	for (i = 0; i != handler->conn_max; i++)
		if (handler->threads[i].thread == thread)
			memset(&handler->threads[i], 0, sizeof(ThreadRegister));
}

void thread_register(ThreadHandler* handler, pthread_t thread, Connection* conn) {
	int i;
	for (i = 0; i != handler->conn_max; i++) {
		pthread_t k = handler->threads[i].thread;
		if (k == 0) {
			handler->threads[i].thread = thread;
			handler->threads[i].conn = conn;
			return;
		}
	}
	
	lerror ("No more threads availiable.");
}

void thread_handler_join_all(ThreadHandler* handler) {
	int i;
	for (i = 0; i != handler->conn_max; i++) {
		pthread_t k = handler->threads[i].thread;
		if (k != 0) {
			memset(&handler->threads[i], 0, sizeof(ThreadRegister));
			pthread_join(k, NULL);
		}
	}
}

void thread_handler_free(ThreadHandler* handler) {
	free(handler->threads);
	free(handler);
}