//
// Created by atuser on 1/6/18.
//

#include <stdlib.h>
#include "hacksaw.h"

static void linked_node_free(LinkedNode* self)
{
    OBJECT_DECREF(self->data);
    free(self);
}

static LinkedNode* linked_node_new(RefObject* object)
{
    LinkedNode* out = malloc(sizeof(LinkedNode));
    out->free = (void (*)(void*)) linked_node_free;

    OBJECT_INCREF(object);
    out->data = object;
    out->next = NULL;
    out->prev = NULL;

    return out;
}

static void linked_vector_free(LinkedVector* self)
{
    LinkedNode* current_ref = self->head;
    LinkedNode* buf_ref = NULL;
    while (current_ref)
    {
        buf_ref = current_ref->next;
        OBJECT_FREE(current_ref);
        current_ref = buf_ref;
    }

    free(self);
}

LinkedVector* linked_vector_new()
{
    LinkedVector* out = malloc(sizeof(LinkedVector));

    out->head = NULL;
    out->free = (void (*)(void*)) linked_vector_free;
    return out;
}

LinkedNode* linked_vector_get(LinkedVector* self, int index)
{
    LinkedNode* current_ref = self->head;
    for (U32 i = 0; i != index && current_ref; i++)
        current_ref = current_ref->next;

    return current_ref;
}

LinkedNode* linked_vector_append(LinkedVector* self, RefObject* data)
{
    LinkedNode* new_node = linked_node_new(data);

    if (!self->head || !self->tail)
    {
        self->head = new_node;
        self->tail = new_node;
    } else
    {
        self->tail->next = new_node;
        new_node->prev = self->tail;
        self->tail = new_node;
    }

    return new_node;
}

LinkedNode* linked_vector_prepend(LinkedVector* self, RefObject* data)
{
    LinkedNode* new_node = linked_node_new(data);

    if (!self->head || !self->tail)
    {
        self->head = new_node;
        self->tail = new_node;
    } else
    {
        self->head->prev = new_node;
        new_node->next = self->head;
        self->head = new_node;
    }

    return new_node;
}
