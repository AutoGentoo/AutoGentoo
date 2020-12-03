//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_PYTHON_UTIL_H
#define AUTOGENTOO_PYTHON_UTIL_H

#include "Python.h"

#define PyInitFunc(name, type) int name(type* self, PyObject* args, PyObject* kwds)
#define PyNewFunc(name) PyObject* name(PyTypeObject* type, PyObject* args, PyObject* kwds)
#define PyMethod(name, type) PyObject* name(type* self, PyObject* args, PyObject* kwds)
#define PyFastMethod(name, type) PyObject* name(type* self, PyObject *const *args, Py_ssize_t nargs)

#endif //AUTOGENTOO_PYTHON_UTIL_H
