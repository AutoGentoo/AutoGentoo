//
// Created by atuser on 9/24/19.
//

#ifndef HACKSAW_SET_H
#define HACKSAW_SET_H

#include "vector.h"
#include "object.h"

typedef struct Set_prv Set;

typedef int (* element_cmp)(void*, void*);

struct Set_prv {
    OBJECT_HEADER
    Vector* parent;
    element_cmp cmp_func;
};

Set* set_new(element_cmp cmp_func);
U32 set_add(Set* set, RefObject* element);
RefObject* set_get(Set* s, U32 i);
I32 set_inside(Set* s, RefObject* item);
Set* set_dup(Set* s);
void set_union(Set* s1, Set* s2);
Set* set_collapse(Set* to_collapse, RefObject* (* merge_func)(RefObject*, RefObject*));
RefObject* set_remove(Set* set, RefObject* element);

#endif //HACKSAW_SET_H
