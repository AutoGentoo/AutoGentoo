//
// Created by atuser on 10/19/17.
//

#include "hacksaw/vector.h"
#include <stdlib.h>
#include <string.h>

Vector* vector_new(vector_opts opts) {
	Vector* out_ptr = malloc(sizeof(Vector));
	out_ptr->s = 32;
	out_ptr->ptr = malloc(sizeof(void*) * out_ptr->s);
	out_ptr->n = 0;
	out_ptr->opts = opts;
	
	return out_ptr;
}

size_t vector_add(Vector* vec, void* el) {
	if (vec->s == (vec->n + 1)) {
		vector_allocate(vec);
	}
	
	vec->ptr[vec->n] = el;
	
	vec->n++;
	return vec->n - 1; // Return index of item
}

void* vector_remove(Vector* vec, int index) {
	if (index >= vec->n)
		return NULL;
	
	void** to_edit = &vec->ptr[index];
	void* out = *to_edit;
	
	if (!(vec->opts & VECTOR_KEEP)) {
		vec->n--;
		
		if (vec->opts & VECTOR_UNORDERED && index != vec->n)
			*to_edit = vec->ptr[vec->n];
		else
			memcpy(&vec->ptr[index], &vec->ptr[index + 1], (vec->n - index) * sizeof(void*));
		
		vec->ptr[vec->n] = NULL;
	}
	else
		*to_edit = NULL;
	
	return out;
}

void vector_insert(Vector* vec, void* el, int index) {
	if (vec->s <= (vec->n + 1))
		vector_allocate(vec);
	
	memmove(&vec->ptr[index + 1], &vec->ptr[index], (vec->n - index) * sizeof(void*));
	vec->ptr[index] = el;
	
	vec->n++;
}

void vector_extend(Vector* dest, Vector* ex) {
	int i;
	for (i = 0; i != ex->n; i++)
		vector_add(dest, vector_get(ex, i));
}

void vector_allocate(Vector* vec) { // A private function
	vec->s *= 2;
	vec->ptr = realloc(vec->ptr, sizeof(void*) * vec->s);
}

void vector_allocate_to_size(Vector* vec, size_t s) {
	vec->s += s;
	vec->ptr = realloc(vec->ptr, sizeof(void*) * vec->s);
}

void* vector_get (Vector* vec, int i) {
	return vec->ptr[i];
}

void vector_free(Vector* vec) {
	if (!vec)
		return;
	
	free(vec->ptr);
	free(vec);
}