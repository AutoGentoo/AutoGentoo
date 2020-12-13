//
// Created by atuser on 12/12/20.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H

typedef struct Package_prv Package;

#include <Python.h>
#include <hacksaw/lut.h>
#include "ebuild.h"


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
extern PyTypeObject PyPackageType;

#endif //AUTOGENTOO_PACKAGE_H
