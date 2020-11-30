//
// Created by atuser on 10/21/17.
//


#include <stdlib.h>
#include <string.h>
#include "hacksaw/string_vector.h"

StringVector* string_vector_new() {
    return vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
}

void string_vector_add(StringVector* vec, char* string) {
    if (!string)
        vector_add(vec, NULL);
    else
        vector_add(vec, strdup(string));
}

void string_vector_insert(StringVector* vec, char* string, int index) {
    vector_insert(vec, strdup(string), index);
}

void string_vector_set(StringVector* vec, char* string, int index) {
    while (vec->s >= index)
        vector_allocate(vec);

    if (vector_get(vec, index))
        free(vector_get(vec, index));

    vec->ptr[index] = string;
}

void string_vector_remove(StringVector* vec, int index) {
    free(string_vector_get(vec, index));
    vector_remove(vec, index);
}

void string_vector_split(StringVector* vec, char* string, char* delim) {
    if (string == NULL)
        return;
    char* buff = strtok(string, delim);

    while (buff != NULL) {
        string_vector_add(vec, buff);
        buff = strtok(NULL, delim);
    }
}

void string_vector_free(StringVector* vec) {
    if (!vec)
        return;

    int i;
    for (i = 0; i != vec->n; i++) {
        free(string_vector_get(vec, i));
    }
    free(vec->ptr);
    free(vec);
}

char* string_vector_get(StringVector* vec, int index) {
    return (char*) vector_get(vec, index);
}
