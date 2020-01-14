//
// Created by atuser on 1/9/20.
//

#ifndef AUTOGENTOO_QUEUE_SET_H
#define AUTOGENTOO_QUEUE_SET_H

#include "queue.h"
#include "set.h"

typedef struct __QueueSet QueueSet;

struct __QueueSet {
	Queue* parent;
	element_cmp cmp;
};

QueueSet* queue_set_new(element_cmp cmp);
int queue_set_inside(QueueSet* q, void* el);
int queue_set_add(QueueSet* q, void* el);
void* queue_set_peek(QueueSet* q);
void* queue_set_pop(QueueSet* q);

void queue_set_foreach(QueueSet* q, void (*f)(void*));

void queue_set_free(QueueSet* q);

#endif //AUTOGENTOO_QUEUE_SET_H
