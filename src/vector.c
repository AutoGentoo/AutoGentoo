//
// Created by atuser on 10/19/17.
//

#include <tools/vector.h>
#include <stdlib.h>
#include <memory.h>

void vector_allocate(Vector* vec);

Vector* vector_new (size_t el_size, vector_opts opts) {
    Vector* out_ptr = malloc(sizeof(Vector));
    out_ptr->s = HACKSAW_VECTOR_SIZE_INC;
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
    memcpy (&vec->ptr[vec->n * vec->size], el, vec->size);
    vec->n++;
}

void vector_remove(Vector* vec, int index) {
    void* zero = malloc (vec->size);

    if (!vec->keep) {
        if (vec->ordered) {
            memcpy(&vec->ptr[index * vec->size],
                        &vec->ptr[(index + 1) * vec->size],
                        vec->size * (vec->n - index)
            ); // Moves everything back by vec->size
        }
        else {
            memcpy(&vec->ptr[index * vec->size],
                        &vec->ptr[vec->n * vec->size],
                        vec->size
            ); // Moves the last element into the open place
        }

        memcpy(&vec->ptr[index*vec->n], zero, vec->size);
        vec->n--;
    }
    else {
        memcpy(&vec->ptr[index*vec->size], zero, vec->size);
    }

    free (zero);
}

void vector_insert(Vector* vec, void* el, int index) {
    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }
}

void vector_allocate(Vector* vec) { // A private function
    vec->s += HACKSAW_VECTOR_SIZE_INC;
    vec->ptr = realloc (vec->ptr, vec->size * vec->s);
}

void vector_free(Vector* vec) {
    free(vec->ptr);
    free(vec);
}