//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_USE_H
#define AUTOGENTOO_USE_H

#include "portage.h"

typedef struct RequiredUse_prv RequiredUse;
typedef struct UseFlag_prv UseFlag;

struct RequiredUse_prv {
    PyObject_HEAD
    char* name;
    Use_t global_flag;
    use_operator_t option;
    RequiredUse* depend; /* Use this if we have a child expression for this operator */
    RequiredUse* next;

    RequiredUse* PyIterator_self__;
};

struct UseFlag_prv {
    PyObject_HEAD
    char* name;
    U8 state;
};

PyNewFunc(PyUseFlag_new);
void use_flag_init(UseFlag* self, const char* name, U8 state);

Use_t use_get_global(Portage* parent, const char* useflag);
RequiredUse* use_build_required_use(const char* target, use_operator_t option);

extern PyTypeObject PyUseFlagType;
extern PyTypeObject PyRequiredUseType;

#endif //AUTOGENTOO_USE_H
