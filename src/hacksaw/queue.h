//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_QUEUE_H
#define AUTOGENTOO_QUEUE_H

#include <stdio.h>
#include "linked_vector.h"

typedef LinkedVector Queue;

/**
 * Vector with add/remove O(1)
 * Access time for start is O(1)
 * Access time for kth-item is O(n) (don't do this)
 */
Queue* queue_new();

void queue_add(Queue* self, RefObject* data);

RefObject* queue_pop(Queue* self);

RefObject* queue_peek(Queue* self);

void queue_foreach(Queue* self, void (* f)(RefObject*));

#endif //AUTOGENTOO_QUEUE_H
