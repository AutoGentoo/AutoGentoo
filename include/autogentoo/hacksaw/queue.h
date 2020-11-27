//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_QUEUE_H
#define AUTOGENTOO_QUEUE_H

#include <stdio.h>

typedef struct __Queue Queue;

struct queue_Node {
    void* data;
    struct queue_Node* next;
};

/**
 * Vector with add/remove O(1)
 * Access time for start is O(1)
 * Access time for kth-item is O(n) (don't do this)
 */
struct __Queue {
    struct queue_Node* head;
    struct queue_Node* tail;
};

Queue* queue_new();

void queue_add(Queue* self, void* data);

void* queue_pop(Queue* self);

void* queue_peek(Queue* self);

void queue_foreach(Queue* self, void (* f)(void*));

void queue_concat(Queue* dest, Queue* to_add);

void queue_free(Queue* self);

#endif //AUTOGENTOO_QUEUE_H
