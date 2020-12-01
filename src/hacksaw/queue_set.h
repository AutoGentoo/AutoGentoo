//
// Created by atuser on 1/9/20.
//

#ifndef AUTOGENTOO_QUEUE_SET_H
#define AUTOGENTOO_QUEUE_SET_H

#include "queue.h"
#include "set.h"

typedef struct QueueSet_prv QueueSet;

struct QueueSet_prv
{
    OBJECT_HEADER

    Queue* parent;
    element_cmp cmp;
};

QueueSet* queue_set_new(element_cmp cmp);

int queue_set_inside(QueueSet* q, RefObject* el);

int queue_set_add(QueueSet* q, RefObject* el);

RefObject* queue_set_peek(QueueSet* q);

RefObject* queue_set_pop(QueueSet* q);

void queue_set_foreach(QueueSet* q, void (* f)(RefObject*));

#endif //AUTOGENTOO_QUEUE_SET_H
