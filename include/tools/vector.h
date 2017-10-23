//
// Created by atuser on 10/18/17.
//

#ifndef HACKSAW_VECTOR_H
#define HACKSAW_VECTOR_H

#include <stdio.h>

typedef enum __vector_opts vector_opts;
typedef struct __Vector Vector;

#ifndef HACKSAW_VECTOR_INCREMENT
#define HACKSAW_VECTOR_INCREMENT 5
#endif

enum __vector_opts {
    UNORDERED = 0x0, // Any order can be used (most efficient)
    ORDERED = 0x1, // Keep the elements in order (don't use if not needed)
    KEEP = 0x2, // Keep the memory unallocated
    REMOVE = 0x0, // Remove the memory and replace the location
};

struct __Vector {
    void* ptr; // Location where elements are stored
    int n; // Number of filled places
    int s; // Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)
    size_t size; // size of each element

    int increment; // Default is HACKSAW_VECTOR_INCREMENT

    int ordered;
    int keep;
};

Vector* vector_new (size_t el_size, vector_opts opts);
void* vector_add(Vector* vec, void* el);
void vector_remove(Vector* vec, int index);
void vector_insert(Vector* vec, void* el, int index);
void vector_allocate(Vector* vec);
void vector_set_increment(Vector* vec, int inc);
void* vector_get(Vector* vec, int i);
void vector_free(Vector* vec);

#endif //HACKSAW_VECTOR_H
