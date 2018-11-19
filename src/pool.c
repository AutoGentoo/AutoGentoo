//
// Created by atuser on 11/16/18.
//

#include <autogentoo/pool.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools.h>
#include <unistd.h>
#include <signal.h>


PoolHandler* pool_handler_new(int pool_num) {
	PoolHandler* handler = malloc (sizeof(PoolHandler));
	
	pthread_mutex_init(&handler->queue_mutex, NULL);
	pthread_mutex_init(&handler->cond_mutex, NULL);
	pthread_cond_init(&handler->condition, NULL);
	
	handler->queue = NULL;
	handler->pools = malloc(sizeof(Pool*) * pool_num);
	handler->pool_num = pool_num;
	
	for (int pool = 0; pool < pool_num; pool++) {
		handler->pools[pool] = pool_new(handler, pool);
		pthread_t current_pid;
		
		if (pthread_create(&current_pid, NULL, (void* (*) (void*))pool_loop, handler->pools[pool])) {
			lerror("Error creating thread");
			fflush(stdout);
			exit(1);
		}
	}
	
	return handler;
}

Pool* pool_new (PoolHandler* parent, int index) {
	Pool* out = malloc (sizeof(Pool));
	out->kill = 0;
	out->pid = 0;
	out->parent = parent;
	out->index = index;
	
	return out;
}

void pool_handler_add(PoolHandler* handler, pool_function f, void* argument) {
	PoolQueue* new = malloc (sizeof (PoolQueue));
	new->function = f;
	new->stack = argument;
	new->next = NULL;
	
	pthread_mutex_lock(&handler->queue_mutex);
	
	if (handler->queue == NULL)
		handler->queue = new;
	else {
		PoolQueue* end_of_queue = handler->queue;
		while (end_of_queue->next != NULL)
			end_of_queue = end_of_queue->next;
		end_of_queue->next = new;
	}
	
	pthread_mutex_unlock(&handler->queue_mutex);
	
	pthread_mutex_lock(&handler->cond_mutex);
	pthread_cond_signal(&handler->condition);
	pthread_mutex_unlock(&handler->cond_mutex);
}

void pool_loop(Pool* pool) {
	pool->pid = pthread_self();
	pthread_mutex_t* queue_mutex = &pool->parent->queue_mutex;
	pthread_mutex_t* condition_mutex = &pool->parent->cond_mutex;
	pthread_cond_t* condition = &pool->parent->condition;
	
	PoolQueue* current_job = NULL;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	pthread_mutex_lock(condition_mutex);
	pthread_cond_wait(condition, condition_mutex);
	
	while (!pool->kill) {
		pthread_mutex_lock(queue_mutex);
		if (pool->parent->queue == NULL) { // Another process got here first or no more jobs
			pthread_mutex_unlock(queue_mutex);
			
			pthread_mutex_lock(condition_mutex);
			pthread_cond_wait(condition, condition_mutex);
			continue;
		}
		pthread_mutex_unlock(condition_mutex);
		
		current_job = pool->parent->queue;
		pool->parent->queue = current_job->next;
		current_job->next = NULL;
		
		pthread_mutex_unlock(queue_mutex);
		
		(*current_job->function)(current_job->stack, pool->index);
		current_job = NULL;
	}
}

void pool_exit (PoolHandler* target) {
	for (int i = 0; i < target->pool_num; i++) {
		pthread_cancel(target->pools[i]->pid);
		free(target->pools[i]);
	}
	
	pthread_mutex_destroy(&target->queue_mutex);
	pthread_mutex_destroy(&target->cond_mutex);
	pthread_cond_destroy(&target->condition);
}