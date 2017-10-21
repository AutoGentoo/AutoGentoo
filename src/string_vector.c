//
// Created by atuser on 10/21/17.
//


#include <tools/string_vector.h>
#include <stdlib.h>

StringVector* string_vector_new () {
    StringVector* out = malloc (sizeof (StringVector));
    out->size = 0;
    out->increment = HACKSAW_STRING_VECTOR_INCREMENT;
    out->ptr = NULL;
    string_vector_allocate(out);

    out->n = 0;
}

void string_vector_set_increment(StringVector* vec, int inc) {
    vec->increment = inc;
}

void string_vector_add (StringVector* vec, char* string);
void string_vector_allocate(StringVector* vec) {
    vec->size += vec->increment;
    vec->ptr = realloc(vec->ptr, sizeof(char) * vec->size);
}
void string_vector_remove (StringVector* vec, int index);
void string_vector_free (StringVector* vec);
StringIndex* string_index (StringVector* vec, int index);
char* string_vector_get(StringIndex* index);
