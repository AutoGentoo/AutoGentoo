//
// Created by tumbar on 12/2/20.
//

#include "portage.h"
#include "python_util.h"

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

PyMethod(PyPortage_dealloc, Portage)
{
    OBJECT_FREE(self->global_flags);
    OBJECT_FREE(self->packages);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

PyTypeObject PyPortageType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Portage",
        .tp_doc = "General struct to manage portage data",
        .tp_basicsize = sizeof(Portage),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyPortage_new,
        .tp_init = (initproc) PyPortage_init,
        .tp_dealloc = (destructor) PyPortage_dealloc,
};