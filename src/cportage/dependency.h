//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

#include "Python.h"
#include "atom.h"

typedef struct Dependency_prv Dependency;

/**
 * selector? ( child child child ) next_target? ( ... ) depend
 */
struct Dependency_prv {
    PyObject_HEAD
    use_operator_t use_operator;
    Use_t use_condition;

    Atom* atom;
    Dependency* children;
    Dependency* next;

    Dependency* PyIterator_self__;
};

Dependency* dependency_build_use(Portage* portage, const char* use_flag, use_operator_t type, Dependency* children);
Dependency* dependency_build_grouping(Dependency* children);
Dependency* dependency_build_atom(Atom* atom);

extern PyTypeObject PyDependencyType;

#endif //AUTOGENTOO_DEPENDENCY_H
