//
// Created by atuser on 10/21/17.
//


#include <tools/string_vector.h>
#include <stdlib.h>
#include <string.h>

StringVector* string_vector_new () {
    return vector_new(sizeof(char*), ORDERED | REMOVE);
}

void string_vector_add (StringVector* vec, char* string) {
    char* new_ptr = malloc (sizeof(char) * strlen(string) + 1);
    strcpy(new_ptr, string);
    
    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }

    ((char**)vec->ptr)[vec->n] = new_ptr;
    vec->n++;
}

void string_vector_insert(StringVector* vec, char* string, int index) {
    char* new_ptr = malloc (sizeof(char) * strlen(string));
    strcpy(new_ptr, string);

    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }

    int i;
    for (i=vec->n; i>=index; i--) {
        ((char**)vec->ptr)[i] = ((char**)vec->ptr)[i-1];
    }

    ((char**)vec->ptr)[index] = new_ptr;
    vec->n++;
}

void string_vector_remove (StringVector* vec, int index) {
    free(string_vector_get(vec, index));
    vector_remove(vec, index);
}

void string_vector_split(StringVector* vec, char* string, char* delim) {
    if (string == NULL) {
        return;
    }
    char* buff;
    for (buff = strtok(string, delim); buff != NULL; string_vector_add(vec, buff));
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
