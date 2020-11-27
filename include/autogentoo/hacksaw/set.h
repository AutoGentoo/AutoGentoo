//
// Created by atuser on 9/24/19.
//

#ifndef HACKSAW_SET_H
#define HACKSAW_SET_H

#include "vector.h"

typedef struct __Set Set;

typedef int (* element_cmp)(void*, void*);

struct __Set {
    Vector* parent;
    element_cmp cmp_func;
};

Set* set_new(element_cmp cmp_func);

int set_add(Set* set, void* element);

void* set_get(Set* s, int i);

int set_inside(Set* s, void* item);

Set* set_dup(Set* s);

void set_union(Set* s1, Set* s2);

Set* set_collapse(Set* to_collapse, void* (* merge_func)(void*, void*));

void set_free(Set* ptr);

void* set_remove(Set* set, void* element);

#endif //HACKSAW_SET_H
