//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "structmember.h"
#include "atom.h"
#include "dependency.h"
#include "use.h"

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

    if (PyType_Ready(&PyDependencyType) < 0
        || PyType_Ready(&PyAtomType) < 0
        || PyType_Ready(&PyAtomVersionType) < 0
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
    Py_INCREF(&PyPortageType);
    if (PyModule_AddObject(m, "Dependency", (PyObject*) &PyDependencyType) < 0
        || PyModule_AddObject(m, "AtomVersion", (PyObject*) &PyAtomVersionType) < 0
        || PyModule_AddObject(m, "Atom", (PyObject*) &PyAtomType) < 0
        || PyModule_AddObject(m, "Portage", (PyObject*) &PyPortageType) < 0
        || PyModule_AddObject(m, "PyUseFlagType", (PyObject*) &PyUseFlagType) < 0
        || PyModule_AddObject(m, "RequiredUse", (PyObject*) &PyRequiredUseType) < 0
        )
    {
        assert(0);
        PyErr_Print();
        Py_DECREF(&PyDependencyType);
        Py_DECREF(&PyAtomType);
        Py_DECREF(&PyPortageType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}