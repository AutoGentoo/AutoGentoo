//
// Created by tumbar on 12/1/20.
//

#include <Python.h>
#include "tcp_server.h"
#include "message.h"
#include "tcp_client.h"
#include "python_util.h"

static PyMethod(Py_send_message, PyObject)
{
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
            return NULL;
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
    int res = 0;
    Py_BEGIN_ALLOW_THREADS
        res = tcp_client_send_message(address, type, &message, &reply);
    Py_END_ALLOW_THREADS

    if (res != 0)
        Py_RETURN_NONE;

    /* Convert the reply to python object */
    PyObject* py_reply = PyMessage_FromMessageFrame(&reply);

    if (reply.size)
        free(reply.data);

    return py_reply;
}

static PyStructSequence_Field PyMessage_Fields[] = {
        {.name="token", "Request token usually stored in enum"},
        {.name="val1", "Parameter 1 (8 bytes)"},
        {.name="val2", "Parameter 2 (8 bytes)"},
        {.name="val3", "Parameter 3 (8 bytes)"},
        {.name="val4", "Parameter 4 (8 bytes)"},
        {.name="val5", "Parameter 5 (8 bytes)"},
        {.name="val6", "Parameter 6 (8 bytes)"},
        {.name="data", "Arbitrarily sized data"},
        {NULL, NULL}
};

static PyStructSequence_Desc PyMessage_Desc = {
        .name = "autogentoo_network.Message",
        .doc = "Model a message structure",
        .fields = PyMessage_Fields,
        .n_in_sequence=8
};

static PyMethodDef module_methods[] = {
        {"send_message", (PyCFunction) Py_send_message, METH_KEYWORDS | METH_VARARGS, "Send a message to a TCP server"},
        {NULL, NULL,                                    0, NULL}
};

static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        .m_name = "autogentoo_network",
        .m_doc = "Multithreaded TCP IPC protocol",
        .m_size = -1,
        module_methods
};

PyMODINIT_FUNC
PyInit_autogentoo_network(void)
{
    PyObject* m = NULL;

    m = PyModule_Create(&module);
    if (m == NULL)
        return NULL;

    if (PyStructSequence_InitType2(&PyMessageType, &PyMessage_Desc) != 0)
    {
        PyErr_Format(PyExc_ImportError, "Failed to initialize Message");
        Py_DECREF(m);
        return NULL;
    }


    if (PyType_Ready(&TCPServerType) < 0
        || PyType_Ready(&PyMessageType) < 0
    )
    {
        Py_DECREF(m);
        return NULL;
    }

    Py_INCREF(&TCPServerType);
    Py_INCREF(&PyMessageType);
    if (PyModule_AddObject(m, "TCPServer", (PyObject*) &TCPServerType) < 0
        || PyModule_AddObject(m, "Message", (PyObject*) &PyMessageType) < 0)
    {
        PyErr_Print();
        Py_DECREF(&TCPServerType);
        Py_DECREF(&PyMessageType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
