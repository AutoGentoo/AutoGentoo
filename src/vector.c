//
// Created by atuser on 10/19/17.
//

#include <tools/vector.h>
#include <stdlib.h>
#include <memory.h>

void vector_allocate(Vector* vec);

Vector* vector_new (size_t el_size, vector_opts opts) {
    Vector* out_ptr = malloc(sizeof(Vector));
    out_ptr->s = HACKSAW_VECTOR_INCREMENT;
    out_ptr->increment = HACKSAW_VECTOR_INCREMENT;
    out_ptr->size = el_size;
    out_ptr->ptr = malloc (out_ptr->size * out_ptr->s);
    out_ptr->n = 0;
    out_ptr->keep = !!(opts & KEEP);
    out_ptr->ordered = !!(opts & ORDERED);

    return out_ptr;
}

void vector_add(Vector* vec, void* el) {
    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }
    memcpy (vector_get(vec, vec->n), el, vec->size);
    vec->n++;
}

void vector_remove(Vector* vec, int index) {
    void* zero = malloc (vec->size);
    memset(zero, 0, vec->size);

    if (!vec->keep) {
        if (vec->ordered) {
            memcpy(vector_get(vec, index),
                   vector_get(vec, index + 1),
                   vec->size * (vec->n - index)
            ); // Moves everything back by vec->size
        }
        else {
            memcpy(vector_get(vec, index),
                   vector_get(vec, vec->n),
                        vec->size
            ); // Moves the last element into the open place
        }

        memcpy(vector_get(vec, vec->n), zero, vec->size);
        vec->n--;
    }
    else {
        memcpy(vector_get(vec, index), zero, vec->size);
    }

    free (zero);
}

void vector_insert(Vector* vec, void* el, int index) {
    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }
    memmove(vector_get (vec, index) + vec->size, vector_get(vec, index), (vec->n - index) * vec->size);
    memcpy(vector_get (vec, index), el, vec->size);
    vec->n++;
}

void vector_allocate(Vector* vec) { // A private function
    vec->s += vec->increment;
    vec->ptr = realloc (vec->ptr, vec->size * vec->s);
}

void* vector_get(Vector* vec, int i) {
    return (void*) &(((char*)vec->ptr)[i * vec->size]); // Casting to get rid of warnings
}

void vector_free(Vector* vec) {
    free(vec->ptr);
    free(vec);
}