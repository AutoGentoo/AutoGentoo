//
// Created by atuser on 10/21/17.
//


#include <tools/string_vector.h>
#include <stdlib.h>
#include <string.h>

StringVector* string_vector_new () {
    return vector_new(sizeof(char*), ORDERED | REMOVE);
}

void string_vector_set_increment(StringVector* vec, int inc) {
    vec->increment = inc;
}

void string_vector_add (StringVector* vec, char* string) {
    char* new_ptr = malloc (sizeof(char) * strlen(string));
    strcpy(new_ptr, string);

    vector_add(vec, new_ptr);
}

void string_vector_remove (StringVector* vec, int index) {
    free(string_vector_get(vec, index));
    vector_remove(vec, index);
}

void string_vector_free (StringVector* vec) {
    int i;
    for (i=0; i!=vec->n; i++) {
        free(string_vector_get(vec, i));
    }
    free(vec->ptr);
    free(vec);
}

char* string_vector_get(StringVector* vec, int index) {
    return *(char**)vector_get(vec, index);
}
