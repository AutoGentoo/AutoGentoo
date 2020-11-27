//
// Created by atuser on 5/18/18.
//


#include <autogentoo/queue.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int prv_handle_item(void* dest, void* data, char type, size_t item_size, void* end);

WorkerQueue* queue_new(queue_t type, char* template, ...) {
    WorkerQueue* out = malloc(sizeof(WorkerQueue));
    out->type = type;
    out->template = strdup(template);
    out->size = 0;

    va_list args;
    va_start (args, template);

    //out->args = malloc (get_item_size (template));
    //size_t offset = 0;
    /*
    for (char* i = template; *i; i++) {
        if (*i == 'i')
            *(int*)(out->args + offset) = va_arg(args, int);
        else if (*i == 's')
            *(char**)(out->args + offset) = strdup(va_arg(args, char*));
        offset += get_sizeof(*i);
    }
    */
    va_end (args);
    return out;
}

WorkerQueue** queue_find_end(WorkerQueue* q) {
    WorkerQueue* end = q;
    while (end->last)
        end = end->last;
    return &(end->last);
}

WorkerQueue* queue_add(WorkerParent* dest, WorkerQueue* src) {
    dest->tail->last = src;
    dest->tail = src;

    return src;
}

WorkerQueue* queue_shrink(WorkerQueue* q) {
    WorkerQueue* out = q->last;
    q->last = NULL;
    return out;
}

void prv_write_template(char* t, void* ptr, int fd) {
    size_t offset = 0;
    size_t len = 0;
    while (*t) {
        if (*t == 'i')
            write(fd, (char*) ptr + offset, len = sizeof(int));
        else if (*t == 's')
            write(fd, (char*) ptr + offset, len = strlen((char*) ptr + offset));
        offset += len;
        t++;
    }
}

void queue_write(WorkerQueue* q, int fd) {
    while (q) {
        write(fd, &q->type, sizeof(int));
        write(fd, q->template, strlen(q->template));
        prv_write_template(q->template, q->args, fd);
        q = q->last;
    }

    int k = QUEUE_END;
    write(fd, &k, sizeof(int));
}

WorkerQueue* queue_free_single(WorkerQueue* q) {
    free(q->args);
    free(q->template);
    WorkerQueue* out = q->last;
    free(q);

    return out;
}

void queue_free(WorkerQueue* head) {
    while (head)
        head = queue_free_single(head);
}
