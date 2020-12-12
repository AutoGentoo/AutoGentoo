//
// Created by tumbar on 12/3/20.
//

#ifndef AUTOGENTOO_MODULE_H
#define AUTOGENTOO_MODULE_H

#include "Python.h"
#include "python_util.h"

PyMODINIT_FUNC PyInit_autogentoo_cportage(void);
PyFastMethod(PyCportage_Init, PyObject*);

#endif //AUTOGENTOO_MODULE_H
