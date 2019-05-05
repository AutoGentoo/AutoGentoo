//
// Created by atuser on 10/19/17.
//

#include <autogentoo/hacksaw/vector.h>
#include <stdlib.h>
#include <memory.h>

Vector* vector_new(size_t el_size, vector_opts opts) {
	Vector* out_ptr = malloc(sizeof(Vector));
	out_ptr->s = 32;
	out_ptr->size = el_size;
	out_ptr->ptr = malloc(out_ptr->size * out_ptr->s);
	out_ptr->n = 0;
	out_ptr->opts = opts;
	
	return out_ptr;
}

size_t vector_add(Vector* vec, void* el) {
	if (vec->s == (vec->n + 1)) {
		vector_allocate(vec);
	}
	void* out = vector_get(vec, (int)vec->n);
	memcpy(out, el, vec->size);
	
	vec->n++;
	return vec->n - 1; // Return index of item
}

void vector_remove(Vector* vec, int index) {
	if (!(vec->opts & VECTOR_KEEP)) {
		if (vec->opts & VECTOR_KEEP) {
			memcpy(vector_get(vec, index),
				   vector_get(vec, index + 1),
				   vec->size * (vec->n - index)
			); // Moves everything back by vec->size
			memset(vector_get(vec, (int)vec->n), 0, vec->size);
		} else {
			memmove(vector_get(vec, index),
					vector_get(vec, (int)vec->n - 1),
					vec->size
			); // Moves the last element into the open place
		}
		vec->n--;
	}
	else
		memset(vector_get(vec, index), 0, vec->size);
}

void vector_insert(Vector* vec, void* el, int index) {
	if (vec->s == (vec->n + 1)) {
		vector_allocate(vec);
	}
	memmove(vector_get(vec, index) + vec->size, vector_get(vec, index), (vec->n - index) * vec->size);
	memcpy(vector_get(vec, index), el, vec->size);
	vec->n++;
}

void vector_extend(Vector* dest, Vector* ex) {
	int i;
	for (i = 0; i != ex->n; i++) {
		vector_add(dest, vector_get(ex, i));
	}
}

void vector_allocate(Vector* vec) { // A private function
	vec->s *= 2;
	vec->ptr = realloc(vec->ptr, vec->size * vec->s);
}

void vector_allocate_to_size(Vector* vec, size_t s) {
	vec->s += s;
	vec->ptr = realloc(vec->ptr, vec->size * vec->s);
}

void** vector_get (Vector* vec, int i) {
	return (void*) &(((char*) vec->ptr)[i * vec->size]); // Casting to get rid of warnings
}

void vector_free(Vector* vec) {
	free(vec->ptr);
	free(vec);
}