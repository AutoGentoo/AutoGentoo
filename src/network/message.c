//
// Created by tumbar on 12/1/20.
//

#include "message.h"

static U64 PyBytes_AsLong(PyObject* self)
{
    if (!PyBytes_Check(self) || PyObject_Length(self) != 8)
        return 0;

    U64* ptr = (U64*)PyBytes_AsString(self);
    return *ptr;
}

PyObject* PyMessage_FromMessageFrame(MessageFrame* self)
{

    PyObject* out = NULL;
    if (self->size)
    {
        out = PyTuple_New(8);
        PyTuple_SetItem(out, 7, PyBytes_FromStringAndSize(self->data, self->size));
    } else
    {
        out = PyTuple_New(7);
    }

    PyTuple_SetItem(out, 0, PyLong_FromLong(self->parent.token));
    PyTuple_SetItem(out, 1, PyBytes_FromStringAndSize((const char*) &self->parent.data.val1, sizeof(U64)));
    PyTuple_SetItem(out, 2, PyBytes_FromStringAndSize((const char*) &self->parent.data.val2, sizeof(U64)));
    PyTuple_SetItem(out, 3, PyBytes_FromStringAndSize((const char*) &self->parent.data.val3, sizeof(U64)));
    PyTuple_SetItem(out, 4, PyBytes_FromStringAndSize((const char*) &self->parent.data.val4, sizeof(U64)));
    PyTuple_SetItem(out, 5, PyBytes_FromStringAndSize((const char*) &self->parent.data.val5, sizeof(U64)));
    PyTuple_SetItem(out, 6, PyBytes_FromStringAndSize((const char*) &self->parent.data.val6, sizeof(U64)));
}

int PyMessage_AsMessageFrame(PyObject* self, MessageFrame* dest)
{
    if (!PyTuple_Check(self)
        || !(PyObject_Length(self) == 7 || PyObject_Length(self) == 8))
    {
        return 1;
    } else
    {
        if (PyObject_Length(self) == 7)
        {
            /* Reply with a Message */
            dest->data = NULL;
            dest->size = 0;
        } else if (PyObject_Length(self) == 8)
        {
            PyBytes_AsStringAndSize(PyTuple_GetItem(self, 7), (char**) &dest->data, (I64*) &dest->size);
        }

        dest->parent.token = PyLong_AsLong(PyTuple_GetItem(self, 0));
        dest->parent.data.val1 = PyBytes_AsLong(PyTuple_GetItem(self, 1));
        dest->parent.data.val2 = PyBytes_AsLong(PyTuple_GetItem(self, 2));
        dest->parent.data.val3 = PyBytes_AsLong(PyTuple_GetItem(self, 3));
        dest->parent.data.val4 = PyBytes_AsLong(PyTuple_GetItem(self, 4));
        dest->parent.data.val5 = PyBytes_AsLong(PyTuple_GetItem(self, 5));
        dest->parent.data.val6 = PyBytes_AsLong(PyTuple_GetItem(self, 6));
    }

    return 0;
}