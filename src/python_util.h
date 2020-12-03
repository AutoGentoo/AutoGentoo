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

#define PyParseMethod(name, type, function) PyFastMethod(name, type) { \
    if (nargs != 1)                     \
    {                                   \
        PyErr_SetString(PyExc_TypeError, "Depedency.parse() requires one argument"); \
        return NULL;                    \
    }                                   \
    PyObject* string = args[0];         \
    if (!PyUnicode_Check(string))       \
    {                                   \
        PyErr_SetString(PyExc_TypeError, "Argument 2 must be a subclass of str"); \
        return NULL;                    \
    }                                   \
    PyObject* out = (PyObject*) function(PyUnicode_AsUTF8(args[0]));   \
    if (!out)                                                          \
    { \
        PyErr_SetString(PyExc_RuntimeError, "Failed to parse string"); \
        return NULL;                                                   \
    }                                                                  \
    return out;                                                        \
}

#endif //AUTOGENTOO_PYTHON_UTIL_H
