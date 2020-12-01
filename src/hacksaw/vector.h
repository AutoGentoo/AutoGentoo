//
// Created by atuser on 10/18/17.
//

#ifndef HACKSAW_VECTOR_H
#define HACKSAW_VECTOR_H

#include <stdio.h>
#include "object.h"

typedef struct Vector_prv Vector;

typedef enum
{
    VECTOR_UNORDERED = 0x0, // Any order can be used (most efficient)
    VECTOR_ORDERED = 0x1, // Keep the elements in order (don't use if not needed)
    VECTOR_KEEP = 0x2, // Keep the memory unallocated
    VECTOR_REMOVE = 0x0, // Remove the memory and replace the location
} vector_opts;

struct Vector_prv
{
    OBJECT_HEADER

    RefObject** ptr; // Location where elements are stored
    int n; // Number of filled places
    size_t s; // Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)

    vector_opts opts;
};

Vector* vector_new(vector_opts opts);

U32 vector_add(Vector* vec, RefObject* el);

RefObject* vector_remove(Vector* vec, U32 index);

void vector_insert(Vector* vec, RefObject* el, U32 index);

void vector_extend(Vector* dest, Vector* ex);

void vector_allocate(Vector* vec);

void vector_allocate_to_size(Vector* vec, U32 s);

RefObject* vector_get(Vector* vec, U32 i);

void vector_foreach(Vector* vec, void (* f)(void*));

#endif //HACKSAW_VECTOR_H
