//
// Created by tumbar on 12/1/20.
//

#include <Python.h>
#include "tcp_server.h"
#include "message.h"
#include "tcp_client.h"

static PyObject* Py_send_message(PyObject* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {"address", "message", NULL};

    PyObject *py_address = NULL, *py_message = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO!", kwlist,
                                     &py_address,
                                     &PyTuple_Type, &py_message))
    {
        return NULL;
    }

    MessageFrame message;
    MessageFrame reply;
    net_type_t type = NETWORK_TYPE_NET;
    Address address;

    /* Parse the address */
    if (PyTuple_Check(py_address))
    {
        if (PyObject_Length(py_address) != 2)
        {
            PyErr_Format(PyExc_TypeError, "Expected address tuple to be of length 2");
            Py_RETURN_NONE;
        }

        type = NETWORK_TYPE_NET;
        address.net_addr.ip = PyLong_AsLong(PyTuple_GetItem(py_address, 0));
        address.net_addr.port = PyLong_AsLong(PyTuple_GetItem(py_address, 1));
    }
    else if (PyUnicode_Check(py_address))
    {
        type = NETWORK_TYPE_UNIX;
        address.path = (char*)PyUnicode_AsUTF8(py_address);
    }
    else
    {
        PyErr_Format(PyExc_TypeError, "Expected a tuple or str for address");
        Py_RETURN_NONE;
    }

    /* Parse the message */
    PyMessage_AsMessageFrame(py_message, &message);

    /* Send the request and wait for reply */
    Py_BEGIN_ALLOW_THREADS
        if (tcp_client_send_message(address, type, &message, &reply) != 0)
        {
            Py_RETURN_NONE;
        }
    Py_END_ALLOW_THREADS

    /* Convert the reply to python object */
    PyObject* py_reply = PyMessage_FromMessageFrame(&reply);

    if (reply.size)
        free(reply.data);

    return py_reply;
}

static PyMethodDef module_methods[] = {
        {"send_message", (PyCFunction) Py_send_message, METH_KEYWORDS | METH_VARARGS, "Send a message to a TCP server"},
        {NULL, NULL,                                    0, NULL}
};

static PyModuleDef module_network = {
        PyModuleDef_HEAD_INIT,
        .m_name = "autogentoo_network",
        .m_doc = "CUDA processing for qMAPP data.",
        .m_size = -1,
        module_methods
};

PyMODINIT_FUNC
PyInit_interferometry_processing(void)
{
    PyObject* m = NULL;
    if (PyType_Ready(&TCPServerType) < 0)
        return NULL;

    m = PyModule_Create(&module_network);
    if (m == NULL)
        return NULL;

    Py_INCREF(&TCPServerType);
    if (PyModule_AddObject(m, "TCPServer", (PyObject*) &TCPServerType) < 0)
        goto error;

    return m;

    error:
    Py_DECREF(&TCPServerType);
    Py_DECREF(m);

    return NULL;
}
