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

void** vector_add(Vector* vec, void* el) {
	if (vec->s == (vec->n + 1)) {
		vector_allocate(vec);
	}
	
	vec->ptr[vec->n] = el;
	return &vec->ptr[vec->n++]; // Return index of item
}

/* Set index to NULL */
void* prv_vector_keep_remove(Vector* vec, int index) {
	void* out = vec->ptr[index];
	vec->ptr[index] = NULL;
	return out;
}

void* prv_vector_remove_unordered(Vector* vec, int index) {
	void* last_data = vec->ptr[vec->n - 1];
	void* out_data = vec->ptr[index];
	
	vec->ptr[index] = last_data;
	vec->ptr[vec->n - 1] = NULL;
	
	return out_data;
}

void* prv_vector_remove_ordered(Vector* vec, int index) {
	void* out_data = vec->ptr[index];
	memcpy(&vec->ptr[index], &vec->ptr[index + 1], (vec->n - index) * sizeof(void*));
	
	return out_data;
}

void* vector_remove(Vector* vec, int index) {
	if (index >= vec->n)
		return NULL;
	
	if (vec->opts & VECTOR_KEEP)
		return prv_vector_keep_remove(vec, index);
	
	vec->n--;
	if (vec->opts & VECTOR_UNORDERED)
		return prv_vector_remove_unordered(vec, index);
	
	return prv_vector_remove_ordered(vec, index);
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
