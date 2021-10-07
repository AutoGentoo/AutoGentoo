//
// Created by tumbar on 9/17/21.
//

#include <Python.h>
#include "python_util.h"
#include "math.h"

PyMethod(Py_calculate_crc32, PyObject)
{
    Py_buffer bytes;
    if (!PyArg_ParseTuple(args, "y*", &bytes))
    {
        // Propagate error
        return NULL;
    }

    return PyLong_FromUnsignedLong(calculate_crc32(bytes.buf, bytes.len));
}

static PyMethodDef module_methods[] = {
        {"calculate_crc32", (PyCFunction) Py_calculate_crc32, METH_KEYWORDS, "Initialize the global portage struct"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "autogentoo_cutil",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        module_methods
};

PyMODINIT_FUNC
PyInit_autogentoo_cutil(void)
{
    (void)PyInit_autogentoo_cutil;

    PyObject* m;

    m = PyModule_Create(&module);
    if (m == NULL)
    {
        return NULL;
    }

    return m;
}
