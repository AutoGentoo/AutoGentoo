//
// Created by atuser on 10/21/17.
//


#include <stdlib.h>
#include <string.h>
#include "string_vector.h"
#include "vector.h"

static void string_vector_free(StringVector* self)
{
    if (!self)
        return;

    for (U32 i = 0; i != self->n; i++)
    {
        free(string_vector_get(self, i));
    }
    free(self->ptr);
    free(self);
}

StringVector* string_vector_new()
{
    StringVector* self = malloc(sizeof(StringVector));

    self->free = (void (*)(void*)) string_vector_free;
    self->s = 32;
    self->n = 0;
    self->ptr = malloc(sizeof(char*) * self->s);

    return self;
}

void string_vector_add(StringVector* vec, const char* string)
{
    if (vec->s == (vec->n + 1))
    {
        vector_allocate((Vector*) vec);
    }

    vec->ptr[vec->n++] = strdup(string);
}

void string_vector_insert(StringVector* vec, const char* string, U32 index)
{
    if (vec->s <= (vec->n + 1))
        vector_allocate((Vector*) vec);

    memmove(&vec->ptr[index + 1], &vec->ptr[index], (vec->n - index) * sizeof(void*));
    vec->ptr[index] = strdup(string);
    vec->n++;
}

void string_vector_set(StringVector* vec, const char* string, U32 index)
{
    while (vec->s >= index)
        vector_allocate((Vector*) vec);

    char* old_value = (char*) vector_get((Vector*) vec, index);
    if (old_value)
        free(old_value);

    vec->ptr[index] = strdup(string);
}


void* prv_vector_remove_ordered(Vector* vec, int index);

void string_vector_remove(StringVector* vec, U32 index)
{
    char* out = prv_vector_remove_ordered((Vector*) vec, index);
    free(out);
}

void string_vector_split(StringVector* vec, char* string, const char* delim)
{
    if (string == NULL)
        return;

    char* buff = strtok(string, delim);

    while (buff != NULL)
    {
        string_vector_add(vec, buff);
        buff = strtok(NULL, delim);
    }
}

char* string_vector_get(StringVector* vec, U32 index)
{
    return (char*) vector_get((Vector*) vec, index);
}
