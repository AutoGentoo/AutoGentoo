//
// Created by atuser on 12/30/19.
//

#include <stdio.h>
#include "stack.h"

Stack* stack_new() {
    return linked_vector_new();
}

void stack_add(Stack* self, RefObject* data) {
    linked_vector_prepend(self, data);
}

RefObject* stack_peek(Stack* self) {
    if (!self->head)
        return NULL;

    return self->head->data;
}

RefObject* stack_pop(Stack* self) {
    if (!self->head)
        return NULL;

    LinkedNode* node = self->head;
    RefObject* out = node->data;
    OBJECT_INCREF(out);

    self->head = node->next;
    OBJECT_FREE(node);

    return out;
}

void stack_foreach(Stack* self, void (* f)(RefObject*)) {
    for (LinkedNode* current = self->head; current; current = current->next)
        f(current->data);
}
