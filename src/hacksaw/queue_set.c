//
// Created by atuser on 1/9/20.
//

#include <stdlib.h>
#include <hacksaw/queue_set.h>

int cmp_default(void* e1, void* e2) {
    return e1 == e2;
}

QueueSet* queue_set_new(element_cmp cmp) {
    QueueSet* out = malloc(sizeof(QueueSet));

    out->parent = queue_new();
    out->cmp = cmp ? cmp : cmp_default;

    return out;
}

int queue_set_inside(QueueSet* q, void* el) {
    for (struct queue_Node* current = q->parent->head; current; current = current->next) {
        if (q->cmp(current->data, el))
            return 1;
    }

    return 0;
}

int queue_set_add(QueueSet* q, void* el) {
    if (queue_set_inside(q, el))
        return 0;

    queue_add(q->parent, el);
    return 1;
}

void* queue_set_peek(QueueSet* q) {
    return queue_peek(q->parent);
}

void* queue_set_pop(QueueSet* q) {
    return queue_pop(q->parent);
}

void queue_set_foreach(QueueSet* q, void (* f)(void*)) {
    queue_foreach(q->parent, f);
}

void queue_set_free(QueueSet* q) {
    queue_free(q->parent);
    free(q);
}