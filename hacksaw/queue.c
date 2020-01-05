//
// Created by atuser on 12/30/19.
//

#include <hacksaw/queue.h>
#include <stdlib.h>

Queue* queue_new() {
	Queue* out = malloc(sizeof(Queue));
	
	out->head = NULL;
	out->tail = NULL;
	
	return out;
}

void queue_add(Queue* self, void* data) {
	struct _Node* new_node = malloc(sizeof(struct _Node));
	new_node->data = data;
	new_node->next = NULL;
	
	
	if (self->head) {
		self->head = new_node;
		self->tail = new_node;
	}
	else {
		self->tail->next = new_node;
		self->tail = new_node;
	}
}

void* queue_pop(Queue* self) {
	if (!self->head)
		return NULL;
	
	struct _Node* del_node = self->head;
	self->head = self->head->next;
	
	if (self->tail == del_node)
		self->tail = NULL;
	
	void* out = del_node->data;
	free(del_node);
	
	return out;
}

void* queue_peek(Queue* self) {
	if (!self->head)
		return NULL;
	
	return self->head->data;
}

void queue_foreach(Queue* self, void (*f)(void*)) {
	for (struct _Node* current = self->head; current; current = current->next)
		f(current->data);
}

void queue_concat(Queue* dest, Queue* to_add) {
	dest->tail->next = to_add->head;
	dest->tail = to_add->tail;
	to_add->head = NULL;
}

void queue_free(Queue* self) {
	while (self->head)
		queue_pop(self);
	
	free(self);
}