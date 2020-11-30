//
// Created by atuser on 10/19/17.
//

#include "vector.h"
#include <stdlib.h>
#include <string.h>

void vector_free(Vector* self) {
    if (!self)
        return;

    for (U32 i = 0; i < self->n; i++) {
        OBJECT_DECREF(self->ptr[i]);
    }

    free(self->ptr);
    free(self);
}

Vector* vector_new(vector_opts opts) {
    Vector* out_ptr = malloc(sizeof(Vector));
    out_ptr->s = 32;
    out_ptr->ptr = malloc(sizeof(RefObject*) * out_ptr->s);
    out_ptr->n = 0;
    out_ptr->opts = opts;

    return out_ptr;
}

U32 vector_add(Vector* vec, RefObject* el) {
    if (vec->s == (vec->n + 1)) {
        vector_allocate(vec);
    }

    OBJECT_INCREF(el);
    vec->ptr[vec->n] = el;
    return vec->n++; // Return index of item
}

/* Set index to NULL */
void* prv_vector_keep_remove(Vector* vec, int index) {
    void* out = vec->ptr[index];
    vec->ptr[index] = NULL;
    return out;
}

void* prv_vector_remove_unordered(Vector* vec, int index) {
    void* last_data = vec->ptr[vec->n - 1];
    void* out_data = vec->ptr[index];

    vec->ptr[index] = last_data;
    vec->ptr[vec->n - 1] = NULL;

    return out_data;
}

void* prv_vector_remove_ordered(Vector* vec, int index) {
    void* out_data = vec->ptr[index];
    memcpy(&vec->ptr[index], &vec->ptr[index + 1], (vec->n - index) * sizeof(void*));

    return out_data;
}

RefObject* vector_remove(Vector* vec, U32 index) {
    if (index >= vec->n)
        return NULL;

    RefObject* out = NULL;
    if (vec->opts & VECTOR_KEEP)
        out = prv_vector_keep_remove(vec, index);
    else {
        vec->n--;
        if (vec->opts & VECTOR_UNORDERED)
            out = prv_vector_remove_unordered(vec, index);
        else
            out = prv_vector_remove_ordered(vec, index);
    }

    OBJECT_DECREF(out);
    return out;
}

void vector_insert(Vector* vec, RefObject* el, U32 index) {
    if (vec->s <= (vec->n + 1))
        vector_allocate(vec);

    memmove(&vec->ptr[index + 1], &vec->ptr[index], (vec->n - index) * sizeof(void*));

    OBJECT_INCREF(el);
    vec->ptr[index] = el;
    vec->n++;
}

void vector_extend(Vector* dest, Vector* ex) {
    for (U32 i = 0; i != ex->n; i++)
        vector_add(dest, vector_get(ex, i));
}

void vector_allocate(Vector* vec) { // A private function
    vec->s *= 2;
    vec->ptr = realloc(vec->ptr, sizeof(void*) * vec->s);
}

void vector_allocate_to_size(Vector* vec, U32 s) {
    vec->s += s;
    vec->ptr = realloc(vec->ptr, sizeof(void*) * vec->s);
}

RefObject* vector_get(Vector* vec, U32 i) {
    return vec->ptr[i];
}

void vector_foreach(Vector* vec, void (* f)(void*)) {
    for (int i = 0; i < vec->n; i++)
        f(vector_get(vec, i));
}