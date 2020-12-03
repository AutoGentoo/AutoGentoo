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
    if (nargs != 2)                     \
    {                                   \
        PyErr_SetString(PyExc_TypeError, "Depedency.parse() requires 2 arguments"); \
        return NULL;                    \
    }                                   \
    PyObject* portage = args[0];        \
    PyObject* string = args[1];         \
    if (!PyObject_IsInstance(portage, (PyObject*) &PyPortageType)) \
    {                                   \
        PyErr_SetString(PyExc_TypeError, "Argument 1 must be a subclass of Portage"); \
        return NULL;                    \
    }                                   \
    if (!PyUnicode_Check(string))       \
    {                                   \
        PyErr_SetString(PyExc_TypeError, "Argument 2 must be a subclass of str"); \
        return NULL;                    \
    }                                   \
    return (PyObject*) function((Portage*) portage, PyUnicode_AsUTF8(args[1])); \
}

#endif //AUTOGENTOO_PYTHON_UTIL_H
