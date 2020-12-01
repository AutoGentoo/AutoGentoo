//
// Created by tumbar on 12/1/20.
//

#include <Python.h>
#include "tcp_server.h"

static PyMethodDef module_methods[] = {
        {NULL, NULL, 0, NULL}
};

static PyModuleDef module_network = {
        PyModuleDef_HEAD_INIT,
        .m_name = "autogentoo_network",
        .m_doc = "CUDA processing for qMAPP data.",
        .m_size = -1,
        module_methods
};

PyMODINIT_FUNC
PyInit_interferometry_processing(void)
{
    PyObject* m = NULL;
    if (PyType_Ready(&TCPServerType) < 0)
        return NULL;

    m = PyModule_Create(&module_network);
    if (m == NULL)
        return NULL;

    Py_INCREF(&TCPServerType);
    if (PyModule_AddObject(m, "TCPServer", (PyObject*) &TCPServerType) < 0
            )
        goto error;

    return m;

    error:
    Py_DECREF(&TCPServerType);
    Py_DECREF(m);

    return NULL;
}
