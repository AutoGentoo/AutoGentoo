//
// Created by atuser on 11/16/18.
//

#ifndef AUTOGENTOO_POOL_H
#define AUTOGENTOO_POOL_H

/**
 * Handles a request through queuing on a pool
 */
typedef struct __PoolQueue PoolQueue;
typedef struct __Pool Pool;
typedef struct __PoolHandler PoolHandler;

typedef void (*pool_function) (void*, int);

#include <pthread.h>
#include <stdio.h>
#include <signal.h>

struct __PoolQueue {
	void* stack;
	pool_function function;
	
	PoolQueue* next;
};

struct __Pool {
	PoolHandler* parent;
	int index;
	pthread_t pid;
	int kill; //!< if 1, kill after next schedule
};

struct __PoolHandler {
	Pool** pools;
	pthread_t scheduler;
	
	int kill;
	
	pthread_mutex_t queue_mutex;
	PoolQueue* queue;
	
	pthread_mutex_t cond_mutex;
	pthread_cond_t condition;
};

PoolHandler* pool_handler_new(int pool_num);
Pool* pool_new (PoolHandler* parent, int index);
void pool_handler_add(PoolHandler* handler, pool_function f, void* argument);
void pool_loop(Pool* pool);


#endif //AUTOGENTOO_POOL_H
