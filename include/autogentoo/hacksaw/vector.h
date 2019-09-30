//
// Created by atuser on 10/18/17.
//

#ifndef HACKSAW_VECTOR_H
#define HACKSAW_VECTOR_H

#include <stdio.h>

typedef enum __vector_opts vector_opts;
typedef struct __Vector Vector;

#define HACKSAW_VECTOR_INITIAL 32

enum __vector_opts {
	VECTOR_UNORDERED = 0x0, // Any order can be used (most efficient)
	VECTOR_ORDERED = 0x1, // Keep the elements in order (don't use if not needed)
	VECTOR_KEEP = 0x2, // Keep the memory unallocated
	VECTOR_REMOVE = 0x0, // Remove the memory and replace the location
};

struct __Vector {
	void** ptr; // Location where elements are stored
	size_t n; // Number of filled places
	size_t s; // Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)
	
	vector_opts opts;
};

Vector* vector_new(vector_opts opts);

size_t vector_add(Vector* vec, void* el);

void* vector_remove(Vector* vec, int index);

void vector_insert(Vector* vec, void* el, int index);

void vector_extend(Vector* dest, Vector* ex);

void vector_allocate(Vector* vec);

void vector_allocate_to_size(Vector* vec, size_t s);

void* vector_get(Vector* vec, int i);

void vector_free(Vector* vec);

#endif //HACKSAW_VECTOR_H
