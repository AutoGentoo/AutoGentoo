//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_STRING_VECTOR_H
#define HACKSAW_STRING_VECTOR_H

#include "object.h"

typedef struct StringVector_prv StringVector;

struct StringVector_prv
{
    OBJECT_HEADER

    char** ptr; // Location where elements are stored
    U32 n; // Number of filled places
    U32 s; // Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)
};

StringVector* string_vector_new(void);

void string_vector_add(StringVector* vec, const char* string);

void string_vector_remove(StringVector* vec, U32 index);

void string_vector_insert(StringVector* vec, const char* string, U32 index);

void string_vector_set(StringVector* vec, const char* string, U32 index);

char* string_vector_get(StringVector* vec, U32 index);

void string_vector_split(StringVector* vec, char* string, const char* delim);

#endif //HACKSAW_STRING_VECTOR_H
