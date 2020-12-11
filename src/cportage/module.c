//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "atom.h"
#include "dependency.h"
#include "use.h"
#include "structmember.h"
#include "ebuild.h"

PyFastMethod(PyCportage_Init, PyObject*)
{
    if (nargs != 1)
    {
        PyErr_Format(PyExc_TypeError, "cportage.init() expects a single argument");
        return NULL;
    }
    if (Py_TYPE(args[0]) != &PyPortageType)
    {
        PyErr_Format(PyExc_TypeError, "cportage.init() expects Portage() object");
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
        || PyType_Ready(&PyEbuildType) < 0
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
    Py_INCREF(&PyEbuildType);
    if (PyModule_AddObject(m, "Dependency", (PyObject*) &PyDependencyType) < 0
        || PyModule_AddObject(m, "AtomVersion", (PyObject*) &PyAtomVersionType) < 0
        || PyModule_AddObject(m, "AtomFlag", (PyObject*) &PyAtomFlagType) < 0
        || PyModule_AddObject(m, "Atom", (PyObject*) &PyAtomType) < 0
        || PyModule_AddObject(m, "Portage", (PyObject*) &PyPortageType) < 0
        || PyModule_AddObject(m, "UseFlag", (PyObject*) &PyUseFlagType) < 0
        || PyModule_AddObject(m, "RequiredUse", (PyObject*) &PyRequiredUseType) < 0
        || PyModule_AddObject(m, "Ebuild", (PyObject*) &PyEbuildType) < 0
        )
    {
        PyErr_Print();
        Py_DECREF(&PyDependencyType);
        Py_DECREF(&PyAtomType);
        Py_DECREF(&PyAtomVersionType);
        Py_DECREF(&PyAtomFlagType);
        Py_DECREF(&PyPortageType);
        Py_DECREF(&PyUseFlagType);
        Py_DECREF(&PyRequiredUseType);
        Py_DECREF(&PyEbuildType);
        Py_DECREF(m);
        return NULL;
    }
    
    /* use_operator_t */
    PyModule_AddIntMacro(m, USE_OP_NONE);
    PyModule_AddIntMacro(m, USE_OP_DISABLE);
    PyModule_AddIntMacro(m, USE_OP_ENABLE);
    PyModule_AddIntMacro(m, USE_OP_LEAST_ONE);
    PyModule_AddIntMacro(m, USE_OP_EXACT_ONE);
    PyModule_AddIntMacro(m, USE_OP_MOST_ONE);
    
    /* use_state_t */
    PyModule_AddIntMacro(m, USE_STATE_UNKNOWN);
    PyModule_AddIntMacro(m, USE_STATE_DISABLED);
    PyModule_AddIntMacro(m, USE_STATE_ENABLED);

    /* atom_use_t */
    PyModule_AddIntMacro(m, ATOM_USE_DISABLE);
    PyModule_AddIntMacro(m, ATOM_USE_ENABLE);
    PyModule_AddIntMacro(m, ATOM_USE_ENABLE_IF_ON);
    PyModule_AddIntMacro(m, ATOM_USE_DISABLE_IF_OFF);
    PyModule_AddIntMacro(m, ATOM_USE_EQUAL);
    PyModule_AddIntMacro(m, ATOM_USE_OPPOSITE);

    /* atom_use_default_t */
    PyModule_AddIntMacro(m, ATOM_DEFAULT_NONE);
    PyModule_AddIntMacro(m, ATOM_DEFAULT_ON);
    PyModule_AddIntMacro(m, ATOM_DEFAULT_OFF);

    /* atom_version_t */
    PyModule_AddIntMacro(m, ATOM_VERSION_NONE);
    PyModule_AddIntMacro(m, ATOM_VERSION_E);
    PyModule_AddIntMacro(m, ATOM_VERSION_L);
    PyModule_AddIntMacro(m, ATOM_VERSION_G);
    PyModule_AddIntMacro(m, ATOM_VERSION_REV);
    PyModule_AddIntMacro(m, ATOM_VERSION_ALL);
    PyModule_AddIntMacro(m, ATOM_VERSION_GE);
    PyModule_AddIntMacro(m, ATOM_VERSION_LE);

    /* atom_block_t */
    PyModule_AddIntMacro(m, ATOM_BLOCK_NONE);
    PyModule_AddIntMacro(m, ATOM_BLOCK_SOFT);
    PyModule_AddIntMacro(m, ATOM_BLOCK_HARD);

    /* atom_slot_t */
    PyModule_AddIntMacro(m, ATOM_SLOT_IGNORE);
    PyModule_AddIntMacro(m, ATOM_SLOT_REBUILD);

    /* atom_version_pre_t */
    PyModule_AddIntMacro(m, ATOM_PREFIX_ALPHA);
    PyModule_AddIntMacro(m, ATOM_PREFIX_BETA);
    PyModule_AddIntMacro(m, ATOM_PREFIX_PRE);
    PyModule_AddIntMacro(m, ATOM_PREFIX_RC);
    PyModule_AddIntMacro(m, ATOM_PREFIX_NONE);
    PyModule_AddIntMacro(m, ATOM_PREFIX_P);

    /* arch_t */
    PyModule_AddIntMacro(m, ARCH_AMD64);
    PyModule_AddIntMacro(m, ARCH_X86);
    PyModule_AddIntMacro(m, ARCH_ARM);
    PyModule_AddIntMacro(m, ARCH_ARM64);
    PyModule_AddIntMacro(m, ARCH_HPPA);
    PyModule_AddIntMacro(m, ARCH_IA64);
    PyModule_AddIntMacro(m, ARCH_PPC);
    PyModule_AddIntMacro(m, ARCH_PPC64);
    PyModule_AddIntMacro(m, ARCH_SPARC);
    PyModule_AddIntMacro(m, ARCH_END);

    /* keyword_t */
    PyModule_AddIntMacro(m, KEYWORD_BROKEN);
    PyModule_AddIntMacro(m, KEYWORD_NONE);
    PyModule_AddIntMacro(m, KEYWORD_UNSTABLE);
    PyModule_AddIntMacro(m, KEYWORD_STABLE);

    return m;
}