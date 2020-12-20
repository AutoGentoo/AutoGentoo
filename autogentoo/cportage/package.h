//
// Created by atuser on 12/12/20.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H


#include <Python.h>
#include "python_util.h"
#include "common.h"
#include <hacksaw/lut.h>


struct Package_prv {
    PyObject_HEAD

    lut_id package_id;
    char* category;
    char* name;
    char* key;

    Ebuild* ebuilds;
};

PyFastMethod(PyPackage_add_ebuild, Package);
PyFastMethod(PyPackage_match_atom, Package);
PyNewFunc(PyPackage_new);

#endif //AUTOGENTOO_PACKAGE_H
