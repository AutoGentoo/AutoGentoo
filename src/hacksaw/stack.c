//
// Created by atuser on 12/30/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <hacksaw/stack.h>

Stack* stack_new() {
    Stack* out = malloc(sizeof(Stack));
    out->head_node = NULL;

    return out;
}

void stack_add(Stack* self, void* data) {
    struct stack_Node* new_node = malloc(sizeof(struct stack_Node));
    new_node->next = self->head_node;
    new_node->data = data;

    self->head_node = new_node;
}

void* stack_peek(Stack* self) {
    if (!self->head_node)
        return NULL;

    return self->head_node->data;
}

void* stack_pop(Stack* self) {
    if (!self->head_node)
        return NULL;

    struct stack_Node* del_node = self->head_node;
    self->head_node = del_node->next;

    void* out = del_node->data;
    free(del_node);

    return out;
}

void stack_foreach(Stack* self, void (* f)(void*)) {
    for (struct stack_Node* current = self->head_node; current; current = current->next)
        f(current->data);
}

void stack_free(Stack* self) {
    while (self->head_node)
        stack_pop(self);

    free(self);
}