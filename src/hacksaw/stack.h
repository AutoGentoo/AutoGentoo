//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_STACK_H
#define AUTOGENTOO_STACK_H

#include "linked_vector.h"

typedef LinkedVector Stack;

Stack* stack_new(void);

void stack_add(Stack* self, RefObject* data);

RefObject* stack_pop(Stack* self);

RefObject* stack_peek(Stack* self);

void stack_foreach(Stack* self, void (* f)(RefObject*));

#endif //AUTOGENTOO_STACK_H
