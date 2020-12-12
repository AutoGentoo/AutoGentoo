//
// Created by atuser on 12/30/19.
//

#include "queue.h"
#include <stdlib.h>

Queue* queue_new(void)
{
    return linked_vector_new();
}

void queue_add(Queue* self, RefObject* data)
{
    linked_vector_append(self, data);
}

RefObject* queue_pop(Queue* self)
{
    if (!self->head)
        return NULL;

    LinkedNode* node = self->head;
    self->head = node->next;

    RefObject* to_pop = node->data;
    OBJECT_INCREF(to_pop);
    OBJECT_FREE(node);

    return to_pop;
}

RefObject* queue_peek(Queue* self)
{
    if (!self->head)
        return NULL;

    return self->head->data;
}

void queue_foreach(Queue* self, void (* f)(RefObject*))
{
    for (LinkedNode* current = self->head; current; current = current->next)
        f(current->data);
}