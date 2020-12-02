//
// Created by tumbar on 12/1/20.
//

#include "message.h"
PyTypeObject PyMessageType;

static U64 PyBytes_AsLong(PyObject* self)
{
    if (!PyBytes_Check(self) || PyObject_Length(self) != 8)
        return 0;

    U64* ptr = (U64*)PyBytes_AsString(self);
    return *ptr;
}

PyObject* PyMessage_FromMessageFrame(MessageFrame* self)
{
    PyObject* out = PyStructSequence_New(&PyMessageType);
    if (self->size)
    {
        PyStructSequence_SetItem(out, 7, PyBytes_FromStringAndSize(self->data, self->size));
    } else
    {
        Py_INCREF(Py_None);
        PyStructSequence_SetItem(out, 7, Py_None);
    }

    PyStructSequence_SetItem(out, 0, PyLong_FromLong(self->parent.token));
    PyStructSequence_SetItem(out, 1, PyBytes_FromStringAndSize((const char*) &self->parent.data.val1, sizeof(U64)));
    PyStructSequence_SetItem(out, 2, PyBytes_FromStringAndSize((const char*) &self->parent.data.val2, sizeof(U64)));
    PyStructSequence_SetItem(out, 3, PyBytes_FromStringAndSize((const char*) &self->parent.data.val3, sizeof(U64)));
    PyStructSequence_SetItem(out, 4, PyBytes_FromStringAndSize((const char*) &self->parent.data.val4, sizeof(U64)));
    PyStructSequence_SetItem(out, 5, PyBytes_FromStringAndSize((const char*) &self->parent.data.val5, sizeof(U64)));
    PyStructSequence_SetItem(out, 6, PyBytes_FromStringAndSize((const char*) &self->parent.data.val6, sizeof(U64)));

    return out;
}

int PyMessage_AsMessageFrame(PyObject* self, MessageFrame* dest)
{
    if (!Py_IS_TYPE(self, &PyMessageType))
    {
        memset(dest, 0, sizeof(MessageFrame));
        return 1;
    } else
    {
        dest->parent.token = PyLong_AsLong(PyStructSequence_GetItem(self, 0));
        dest->parent.data.val1 = PyBytes_AsLong(PyStructSequence_GetItem(self, 1));
        dest->parent.data.val2 = PyBytes_AsLong(PyStructSequence_GetItem(self, 2));
        dest->parent.data.val3 = PyBytes_AsLong(PyStructSequence_GetItem(self, 3));
        dest->parent.data.val4 = PyBytes_AsLong(PyStructSequence_GetItem(self, 4));
        dest->parent.data.val5 = PyBytes_AsLong(PyStructSequence_GetItem(self, 5));
        dest->parent.data.val6 = PyBytes_AsLong(PyStructSequence_GetItem(self, 6));

        PyObject* data_ob = PyStructSequence_GetItem(self, 7);
        if (data_ob == Py_None)
        {
            dest->size = 0;
            dest->data = NULL;
        }
        else
        {
            dest->size = PyObject_Length(data_ob);
            dest->data = malloc(dest->size);

            memcpy(dest->data, PyBytes_AsString(data_ob), dest->size);
        }
    }

    return 0;
}