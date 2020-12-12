//
// Created by tumbar on 12/2/20.
//

#include "portage.h"
#include "python_util.h"
#include "package.h"

#ifndef PORTAGE_INITIAL_PKG_LUT_SIZE
#define PORTAGE_INITIAL_PKG_LUT_SIZE 512
#endif

Portage* global_portage = NULL;

PyNewFunc(PyPortage_new)
{
    Portage* self = (Portage*) type->tp_alloc(type, 0);
    self->global_flags = NULL;
    self->packages = NULL;

    return (PyObject*) self;
}

PyInitFunc(PyPortage_init, Portage)
{
    self->global_flags = lut_new(256);
    self->packages = lut_new(PORTAGE_INITIAL_PKG_LUT_SIZE);
    return 0;
}

static
PyMethod(PyPortage_dealloc, Portage)
{
    OBJECT_FREE(self->global_flags);
    OBJECT_FREE(self->packages);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

PyFastMethod(PyPortage_get_package, Portage)
{
    if (nargs != 1)
    {
        PyErr_Format(PyExc_TypeError, "Portage.get_package expects a single argument");
        return NULL;
    }

    U64 package_id = PyLong_AsUnsignedLongLong(args[0]);
    Package* package = (Package*)lut_get(global_portage->packages, package_id);

    if (!package)
        Py_RETURN_NONE;

    Py_INCREF(package);
    return (PyObject*) package;
}

PyFastMethod(PyPortage_add_package, Portage)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyPackageType))
    {
        PyErr_Format(PyExc_TypeError, "Portage.get_package expects a Package");
        return NULL;
    }

    Package* pkg = (Package*) args[0];
    pkg->package_id = lut_insert(self->packages, pkg->key, (U64) pkg, LUT_FLAG_PYTHON);

    Py_RETURN_NONE;
}

PyFastMethod(PyPortage_match_atom, Portage)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyAtomType))
    {
        PyErr_Format(PyExc_TypeError, "Portage.match_atom expects an Atom");
        return NULL;
    }

    Atom* atom = (Atom*) args[0];
    Package* pkg  = (Package*) lut_get(self->packages, atom->id);
    return PyPackage_match_atom(pkg, args, nargs);
}

static PyMethodDef PyPortage_methods[] = {
        {"get_package", (PyCFunction) PyPortage_get_package, METH_FASTCALL, "Find a package in this instance"},
        {"add_package", (PyCFunction) PyPortage_add_package, METH_FASTCALL, "Register a package"},
        {"match_atom", (PyCFunction) PyPortage_match_atom, METH_FASTCALL},
        {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyTypeObject PyPortageType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Portage",
        .tp_doc = "General struct to manage portage data",
        .tp_basicsize = sizeof(Portage),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyPortage_new,
        .tp_methods = PyPortage_methods,
        .tp_init = (initproc) PyPortage_init,
        .tp_dealloc = (destructor) PyPortage_dealloc,
};