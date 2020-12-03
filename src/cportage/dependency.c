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
    Py_XDECREF(self->next);
    Py_XDECREF(self->atom);
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
    self->PyIterator_self__ = NULL;

    return (PyObject*) self;
}


static PyInitFunc(PyDependency_init, Dependency)
{
    static char* kwlist[] = {"depend_string", NULL};
    const char* depend_string = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &depend_string))
        return -1;

    Dependency * duped = depend_parse(depend_string);
    if (!duped)
        return -1;

    memcpy(&self->use_operator, &duped->use_operator, sizeof(Dependency) - offsetof(Dependency , use_operator));
    Py_TYPE(duped)->tp_free((PyObject*) duped);
    return 0;
}

Dependency* dependency_build_atom(Atom* atom)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);
    self->atom = atom;

    Py_XDECREF(self->children);
    self->children = NULL;

    return self;
}

Dependency* dependency_build_grouping(Dependency* children)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);
    self->children = children;

    Py_XDECREF(self->atom);
    self->atom = NULL;

    return self;
}

Dependency* dependency_build_use(const char* use_flag, use_operator_t type, Dependency* children)
{
    Dependency* self = (Dependency*) PyDependency_new(&PyDependencyType, NULL, NULL);
    self->use_condition = use_get_global(global_portage, use_flag);
    self->use_operator = type;

    self->children = children;
    return self;
}

static PyObject* PyDependency_iter(Dependency* self)
{
    Py_INCREF(self);
    self->PyIterator_self__ = self;
    return (PyObject*) self;
}
static PyObject* PyDependency_next(Dependency* self)
{
    if (self->PyIterator_self__)
    {
        Dependency* out = self->PyIterator_self__;
        Py_INCREF(out);
        self->PyIterator_self__ = self->PyIterator_self__->next;
        return (PyObject*) out;
    }

    return NULL;
}

static PyMemberDef PyDependency_members[] = {
        {"use_operator", T_INT, offsetof(Dependency, use_operator), READONLY},
        {"use_condition", T_ULONGLONG, offsetof(Dependency, use_condition), READONLY},
        {"atom", T_OBJECT, offsetof(Dependency, atom), READONLY},
        {"children", T_OBJECT, offsetof(Dependency, children), READONLY},
        {"next", T_OBJECT, offsetof(Dependency, next), READONLY},
        {NULL, 0, 0, 0, NULL}
};


PyTypeObject PyDependencyType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Dependency",
        .tp_doc = "A way to keep track of a dependency expression",
        .tp_basicsize = sizeof(Dependency),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyDependency_new,
        .tp_init = (initproc) PyDependency_init,
        .tp_dealloc = (destructor) PyDependency_dealloc,
        .tp_members = PyDependency_members,
        .tp_iter = (getiterfunc) PyDependency_iter,
        .tp_iternext = (iternextfunc) PyDependency_next
};
