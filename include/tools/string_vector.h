//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_STRING_VECTOR_H
#define HACKSAW_STRING_VECTOR_H

#include "vector.h"

typedef Vector StringVector;

StringVector* string_vector_new ();
void string_vector_add (StringVector* vec, char* string);
void string_vector_remove (StringVector* vec, int index);
void string_vector_insert(StringVector* vec, char* string, int index);
char* string_vector_get(StringVector* vec, int index);
void string_vector_split(StringVector* vec, char* string, char* delim);
void string_vector_free (StringVector* vec);


#endif //HACKSAW_STRING_VECTOR_H
