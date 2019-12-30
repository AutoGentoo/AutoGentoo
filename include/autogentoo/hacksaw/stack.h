//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_STACK_H
#define AUTOGENTOO_STACK_H

typedef struct __Stack Stack;

struct __Stack {
	struct _Node {
		void* data;
		struct _Node* next;
	}* head_node;
};

Stack* stack_new();
void stack_add(Stack* self, void* data);
void* stack_pop(Stack* self);
void stack_foreach(Stack* self, void (*f)(void*));
void stack_free(Stack* self);

#endif //AUTOGENTOO_STACK_H
