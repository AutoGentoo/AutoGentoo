//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "python_util.h"
#include "dependency.h"
#include "use.h"
#include "language.h"
#include <structmember.h>

static PyMethod(PyDependency_dealloc, Dependency)
{
    Py_XDECREF(self->atom);
    Py_XDECREF(self->next);
    Py_XDECREF(self->children);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyNewFunc(PyDependency_new)
{
    Dependency* self = (Dependency*) type->tp_alloc(type, 0);
    self->next = NULL;
    self->children = NULL;
    self->use_condition = 0;
    self->atom = NULL;

    return (PyObject*) self;
}


static PyInitFunc(PyDependency_init, Dependency)
{
    PyErr_SetString(PyExc_Warning, "'Dependency' should not be explcitely __init__'ed");
    PyErr_Print();
    PyErr_Clear();
    return 0;
}

Dependency* dependency_build_atom(Atom* atom)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);

    Py_XINCREF(atom);
    self->atom = atom;

    Py_XDECREF(self->children);
    self->children = NULL;

    return self;
}

Dependency* dependency_build_grouping(Dependency* children)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);

    Py_XINCREF(children);
    self->children = children;

    Py_XDECREF(self->atom);
    self->atom = NULL;

    return self;
}

Dependency* dependency_build_use(Portage* parent,
                                 const char* use_flag,
                                 use_operator_t type,
                                 Dependency* children)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);
    self->use_condition = use_get_global(parent, use_flag);

    Py_XINCREF(children);
    self->children = children;

    return self;
}

static PyObject* PyDependency_iter(PyObject* self) {return self;}
static PyObject* PyDependency_next(Dependency* self)
{
    if (self->next)
        return (PyObject*) self->next;

    Py_RETURN_NONE;
}

static PyParseMethod(PyDependency_parse, Dependency, depend_parse)

static PyMethodDef PyDependency_methods[] = {
        {"parse", (PyCFunction) PyDependency_parse, METH_STATIC | METH_FASTCALL, "parse dependency string"}
};

static PyMemberDef PyDependency_members[] = {
        {"use_operator", T_INT, offsetof(Dependency, use_operator), READONLY},
        {"use_condition", T_ULONGLONG, offsetof(Dependency, use_condition), READONLY},
        {"atom", T_OBJECT, offsetof(Dependency, use_condition), READONLY},
        {"children", T_OBJECT, offsetof(Dependency, use_condition), READONLY},
        {"next", T_OBJECT, offsetof(Dependency, use_condition), READONLY},
        {NULL, 0, 0, 0, NULL}
};


PyTypeObject PyDependencyType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Dependency",
        .tp_doc = "A way to keep trace of a dependecy expression",
        .tp_basicsize = sizeof(Dependency),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyDependency_new,
        .tp_init = (initproc) PyDependency_init,
        .tp_dealloc = (destructor) PyDependency_dealloc,
        .tp_members = PyDependency_members,
        .tp_methods = PyDependency_methods,
        .tp_iter = PyDependency_iter,
        .tp_iternext = (iternextfunc) PyDependency_next
};
