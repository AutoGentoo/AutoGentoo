//
// Created by atuser on 5/18/18.
//

#ifndef AUTOGENTOO_QUEUE_H
#define AUTOGENTOO_QUEUE_H

typedef struct __WorkerQueue WorkerQueue;
typedef struct __WorkerParent WorkerParent;

#include <stdio.h>
#include <unistd.h>

typedef enum {
    QUEUE_END,
    QUEUE_HANDOFF,
    QUEUE_MAKECONF
} queue_t;

struct __WorkerQueue {
    queue_t type;
    char* template;
    void* args;
    size_t size;
    WorkerQueue* last;
};

struct __WorkerParent {
    WorkerQueue* head;
    WorkerQueue* tail;
    pid_t proc_id;
};

WorkerQueue* queue_new(queue_t type, char* template, ...);

WorkerQueue** queue_find_end(WorkerQueue* q);

WorkerQueue* queue_add(WorkerParent* dest, WorkerQueue* src);

WorkerQueue* queue_shrink(WorkerQueue* q);

void queue_write(WorkerQueue* q, int fd);

WorkerQueue* queue_free_single(WorkerQueue* q);

void queue_free(WorkerQueue* head);

#endif //AUTOGENTOO_QUEUE_H
