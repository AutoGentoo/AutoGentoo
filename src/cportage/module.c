//
// Created by tumbar on 12/2/20.
//

#include "Python.h"

static PyMethodDef module_methods[] = {
        {NULL, NULL,0, NULL}
};

static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        .m_name = "autogentoo_cportage",
        .m_doc = "Backend to portage, parses metadata",
        .m_size = -1,
        module_methods
};

PyMODINIT_FUNC
PyInit_autogentoo_cportage(void)
{
    PyObject* m = PyModule_Create(&module);
    if (m == NULL)
        return NULL;

    return m;
}