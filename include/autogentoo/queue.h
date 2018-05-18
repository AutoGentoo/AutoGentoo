//
// Created by atuser on 5/18/18.
//

#ifndef AUTOGENTOO_QUEUE_H
#define AUTOGENTOO_QUEUE_H

#include "request.h"

typedef struct __Queue Queue;

typedef enum {
	QUEUE_END,
	QUEUE_HANDOFF,
	QUEUE_MAKECONF
} queue_t;

struct __Queue {
	queue_t type;
	char* template;
	void* args;
	size_t size;
	Queue* last;
};

Queue* queue_new (queue_t type, char* template, ...);
Queue** queue_find_end (Queue* q);
Queue* queue_add (Queue* dest, Queue* src);
Queue* queue_shrink (Queue* q);
void queue_write (Queue* q, int fd);
Queue* queue_free_single (Queue* q);
void queue_free (Queue* head);

#endif //AUTOGENTOO_QUEUE_H
