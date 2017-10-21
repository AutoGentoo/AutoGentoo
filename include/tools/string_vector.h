//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_STRING_VECTOR_H
#define HACKSAW_STRING_VECTOR_H

#include <tools/vector.h>

typedef Vector StringVector;

struct __StringIndex { // Make sure that we still have a valid ptr even after element removal
    StringVector* parent;
    int index;
};

StringVector* string_vector_new ();
void string_vector_set_increment(StringVector* vec, int inc);
void string_vector_add (StringVector* vec, char* string);
void string_vector_allocate(StringVector* vec);
void string_vector_remove (StringVector* vec, int index);
char* string_vector_get(StringVector* vec, int index);
void string_vector_free (StringVector* vec);


#endif //HACKSAW_STRING_VECTOR_H
