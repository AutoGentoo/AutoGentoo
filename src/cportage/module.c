//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "structmember.h"
#include "atom.h"
#include "dependency.h"
#include "use.h"

PyFastMethod(PyCportage_Init, PyObject*)
{
    if (nargs != 1)
    {
        PyErr_Format(PyExc_TypeError, "cportage.init() expects a single argument");
        return NULL;
    }

    Py_XDECREF(global_portage);
    Py_INCREF(args[0]);
    global_portage = (Portage*) args[0];
    Py_RETURN_NONE;
}

void PyCportage_free()
{
    Py_XDECREF(global_portage);
    global_portage = NULL;
}

static PyMethodDef module_methods[] = {
        {"init", (PyCFunction) PyCportage_Init, METH_FASTCALL, "Initialize the global portage struct"},
        {NULL, NULL,0, NULL}
};

static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        .m_name = "autogentoo_cportage",
        .m_doc = "Backend to portage, parses metadata",
        .m_size = -1,
        .m_free = PyCportage_free,
        .m_methods = module_methods
};

PyMODINIT_FUNC
PyInit_autogentoo_cportage(void)
{
#if !(PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 7)
#error "Python 3.7+ is required"
#endif

    PyObject* m = PyModule_Create(&module);
    if (m == NULL)
        return NULL;

    Py_Initialize();

    if (PyType_Ready(&PyDependencyType) < 0
        || PyType_Ready(&PyAtomType) < 0
        || PyType_Ready(&PyAtomVersionType) < 0
        || PyType_Ready(&PyAtomFlagType) < 0
        || PyType_Ready(&PyPortageType) < 0
        || PyType_Ready(&PyUseFlagType) < 0
        || PyType_Ready(&PyRequiredUseType) < 0
        )
    {
        Py_DECREF(m);
        return NULL;
    }

    Py_INCREF(&PyDependencyType);
    Py_INCREF(&PyAtomType);
    Py_INCREF(&PyAtomVersionType);
    Py_INCREF(&PyAtomFlagType);
    Py_INCREF(&PyPortageType);
    Py_INCREF(&PyUseFlagType);
    Py_INCREF(&PyRequiredUseType);
    if (PyModule_AddObject(m, "Dependency", (PyObject*) &PyDependencyType) < 0
        || PyModule_AddObject(m, "AtomVersion", (PyObject*) &PyAtomVersionType) < 0
        || PyModule_AddObject(m, "AtomFlag", (PyObject*) &PyAtomFlagType) < 0
        || PyModule_AddObject(m, "Atom", (PyObject*) &PyAtomType) < 0
        || PyModule_AddObject(m, "Portage", (PyObject*) &PyPortageType) < 0
        || PyModule_AddObject(m, "PyUseFlagType", (PyObject*) &PyUseFlagType) < 0
        || PyModule_AddObject(m, "RequiredUse", (PyObject*) &PyRequiredUseType) < 0
        )
    {
        PyErr_Print();
        Py_DECREF(&PyDependencyType);
        Py_DECREF(&PyAtomType);
        Py_DECREF(&PyAtomVersionType);
        Py_DECREF(&PyPortageType);
        Py_DECREF(&PyUseFlagType);
        Py_DECREF(&PyRequiredUseType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}