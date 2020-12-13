//
// Created by atuser on 12/12/20.
//

#include "package.h"
#include "structmember.h"

PyNewFunc(PyPackage_new)
{
    Package* self = (Package*) type->tp_alloc(type, 0);
    self->package_id = 0;
    self->category = NULL;
    self->name = NULL;
    self->key = NULL;
    self->ebuilds = NULL;

    return (PyObject*) self;
}

PyInitFunc(PyPackage_init, Package)
{
    static char* kwlist[] = {"key", NULL};

    const char* key;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key))
        return -1;

    char* cat_split = strchr(key, '/');
    self->key = strdup(key);
    self->category = strndup(key, cat_split - key);
    self->name = strdup(cat_split + 1);

    self->ebuilds = NULL;
    return 0;
}

PyMethod(PyPackage_dealloc, Package)
{
    Py_XDECREF(self->ebuilds);
    SAFE_FREE(self->key);
    SAFE_FREE(self->category);
    SAFE_FREE(self->name);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

PyFastMethod(PyPackage_add_ebuild, Package)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyEbuildType))
    {
        PyErr_Format(PyExc_TypeError, "Package.add_ebuild expects an Ebuild");
        return NULL;
    }

    Ebuild* ebuild = (Ebuild*) args[0];
    if (ebuild->older || ebuild->newer)
    {
        PyErr_SetString(PyExc_RuntimeError, "This ebuild has already been added to a package");
        return NULL;
    }

    /* Add the ebuild to the doubly linked list */
    Ebuild* last = NULL;  /* Keep track for double linkage */
    Ebuild* current;

    for (current = self->ebuilds; current; current = current->older)
    {
        if (ebuild == current)
        {
            PyErr_SetString(PyExc_RuntimeError, "Cannot add the same ebuild twice");
            return NULL;
        }

        if (atom_version_compare(ebuild->version, current->version) > 0)
        {
            /* This version is new than the current */
            break;
        }

        last = current;
    }

    if (!current)
    {
        /* This package is the tail */
        ebuild->older = NULL;
    }
    else
    {
        Py_INCREF(current);
        Py_INCREF(ebuild);
        ebuild->older = current;
        current->newer = ebuild;
    }

    if (!last)
    {
        /* This package is the head */
        ebuild->newer = NULL;
        Py_XDECREF(self->ebuilds);
        Py_INCREF(ebuild);
        self->ebuilds = ebuild;
    }
    else
    {
        Py_INCREF(last);
        Py_INCREF(ebuild);
        ebuild->newer = last;
        last->older = ebuild;
    }

    ebuild->package = self;
    Py_RETURN_NONE;
}

PyFastMethod(PyPackage_match_atom, Package)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyAtomType))
    {
        PyErr_Format(PyExc_TypeError, "Package.add_ebuild expects an Atom");
        return NULL;
    }

    Atom* atom = (Atom*) args[0];
    for (Ebuild* current = self->ebuilds; current; current = current->older)
    {
        int compare = atom_version_compare(current->version, atom->version);
        if (compare == 0 && atom->range & ATOM_VERSION_E
            || compare > 0 && atom->range & ATOM_VERSION_G
            || compare < 0 && atom->range & ATOM_VERSION_L)
        {
            Py_INCREF(current);
            return (PyObject*) current;
        }

        /* Short circuit if packages get too old */
        if (compare > 0 && !(atom->range & ATOM_VERSION_L))
            break;
    }

    Py_RETURN_NONE;
}

static PyObject* PyPackage_iter(Package* self)
{
    Py_XINCREF(self->ebuilds);
    return (PyObject*) self->ebuilds;
}

static PyMethodDef PyPackage_methods[] = {
        {"add_ebuild", (PyCFunction) PyPackage_add_ebuild, METH_FASTCALL},
        {"match_atom", (PyCFunction) PyPackage_match_atom, METH_FASTCALL},
        {NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyMemberDef PyPackage_members[] = {
        {"package_id", T_ULONGLONG, offsetof(Package, package_id), READONLY},
        {"category", T_STRING, offsetof(Package, category), READONLY},
        {"name", T_STRING, offsetof(Package, name), READONLY},
        {"key", T_OBJECT, offsetof(Package, key), READONLY},
        {"ebuilds", T_OBJECT, offsetof(Package, ebuilds), READONLY},
        {NULL, 0, 0, 0, NULL}
};

PyTypeObject PyPackageType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Package",
        .tp_doc = "Keep track of a collection of ebuilds",
        .tp_basicsize = sizeof(Package),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyPackage_new,
        .tp_init = (initproc) PyPackage_init,
        .tp_dealloc = (destructor) PyPackage_dealloc,
        .tp_members = PyPackage_members,
        .tp_methods = PyPackage_methods,
        .tp_iter = (getiterfunc) PyPackage_iter,
};
