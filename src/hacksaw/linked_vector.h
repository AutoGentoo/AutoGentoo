//
// Created by atuser on 1/6/18.
//

#ifndef HACKSAW_LINKED_VECTOR_H
#define HACKSAW_LINKED_VECTOR_H

#include "object.h"

typedef struct LinkedNode_prv LinkedNode;
typedef struct LinkedVector_prv LinkedVector;

struct LinkedNode_prv {
    OBJECT_HEADER
    RefObject* data;

    LinkedNode* prev;
    LinkedNode* next;
};

struct LinkedVector_prv {
    OBJECT_HEADER
    LinkedNode* head;
    LinkedNode* tail;
};

LinkedVector* linked_vector_new();
LinkedNode* linked_vector_get(LinkedVector* self, int index);
LinkedNode* linked_vector_append(LinkedVector* self, RefObject* data);
LinkedNode* linked_vector_prepend(LinkedVector* self, RefObject* data);

#endif //HACKSAW_LINKED_VECTOR_H
