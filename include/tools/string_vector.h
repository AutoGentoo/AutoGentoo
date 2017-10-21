//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_STRING_VECTOR_H
#define HACKSAW_STRING_VECTOR_H

#include <tools/vector.h>

typedef struct __StringVector StringVector;
typedef struct __StringIndex StringIndex;

#ifndef HACKSAW_STRING_VECTOR_INCREMENT
#define HACKSAW_STRING_VECTOR_INCREMENT 32
#endif

struct __StringVector {
    void* ptr; // Location where elements are stored
    int n; // Index of the character after the last string
    size_t size; // Current size in bytes (also char)

    int increment; // Default is HACKSAW_STRING_VECTOR_INCREMENT
};

struct __StringIndex { // Make sure that we still have a valid ptr even after element removal
    StringVector* parent;
    int index;
};

StringVector* string_vector_new ();
void string_vector_set_increment(StringVector* vec, int inc);
void string_vector_add (StringVector* vec, char* string);
void string_vector_allocate(StringVector* vec);
void string_vector_remove (StringVector* vec, int index);
void string_vector_free (StringVector* vec);
StringIndex* string_index (StringVector* vec, int index);
char* string_vector_get(StringIndex* index);


#endif //HACKSAW_STRING_VECTOR_H
