//
// Created by tumbar on 11/30/20.
//

#include "tcp.h"
#include "message.h"
#include <structmember.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

static PyObject*
TCPServer_repr(TCPServer* self) {
    PyObject* obj = NULL;
    if (self->type == NETWORK_TYPE_UNIX) {
        obj = PyUnicode_FromFormat("TCPServer<path=%s, workers=%zu>",
                                   self->address.path,
                                   self->worker_n);
    }
    else {
        obj = PyUnicode_FromFormat("TCPServer<port=%zu, workers=%zu>",
                                   self->address.port,
                                   self->worker_n);
    }

    Py_XINCREF(obj);
    return obj;
}

static PyObject*
TCPServer_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    TCPServer* self = (TCPServer*) type->tp_alloc(type, 0);

    self->worker_n = TCP_WORKER_COUNT;
    self->worker_threads = malloc(sizeof(pthread_t) * self->worker_n);

    self->type = NETWORK_TYPE_NET;
    self->address.path = NULL;

    self->request_queue = queue_new();
    self->keep_alive = 0;

    pthread_mutex_init(&self->lock, NULL);
    pthread_cond_init(&self->cond, NULL);

    return (PyObject*) self;
}

static int
TCPServer_init(TCPServer* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {"address", NULL};

    PyObject* py_address = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &py_address))
        return -1;

    if (PyObject_TypeCheck(py_address, &PyUnicode_Type)) {
        /* Unix domain socket */
        self->type = NETWORK_TYPE_UNIX;
        self->address.path = strdup(PyUnicode_AsUTF8(py_address));
    }
    else if (PyObject_TypeCheck(py_address, &PyLong_Type)) {
        /* Use a normal port */
        self->type = NETWORK_TYPE_NET;
        self->address.port = PyLong_AsLong(py_address);
    }
    else {
        /* Invalid input type */
        return -1;
    }

    return 0;
}

static void TCPServer_worker_run(TCPServer* self) {
    while (self->keep_alive) {
        pthread_mutex_lock(&self->lock);
        if (!queue_peek(self->request_queue)) {
            pthread_cond_wait(&self->cond, &self->lock);
            pthread_mutex_unlock(&self->lock);
            continue;
        }

        Request* req = (Request*) queue_pop(self->request_queue);
        pthread_mutex_unlock(&self->lock);

        /* Read the request from the client socket */
        U32 message_length = 0;
        read(req->client, &message_length, 4);

        MessageDataFrame message;
        read(req->client, &message, message_length);

        read(req->client, &message.size, 4);
        if (message.size) {
            message.data = malloc(message.size);
            read(req->client, message.data, message.size);
        }
        else
            message.data = NULL;
    }
}

static int TCPServer_init_network_socket(U16 port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = socket(addr.sin_family, SOCK_STREAM, 0);
    if (listenfd == -1) {
        lerror("socket() error");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
        lerror("setsockopt(SO_REUSEADDR) failed");

    if (bind(listenfd, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
        lerror("socket() or bind()");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    if (listen(listenfd, 64) != 0) {
        lerror("listen() error");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    return listenfd;
}

static int TCPServer_init_unix_socket(const char* path) {
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    int listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenfd == -1) {
        lerror("socket() error");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    unlink(path);

    if (bind(listenfd, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
        lerror("socket() or bind()");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    if (listen(listenfd, 32) != 0) {
        lerror("listen() error");
        lerror("Error [%d] %s", errno, strerror(errno));
        return -1;
    }

    return listenfd;
}

static Request* request_new(int client_sock) {
    Request* self = malloc(sizeof(Request));
    self->free = free;
    self->reference_count = 0;
    self->client = client_sock;
}

static void TCPServer_run(TCPServer* self) {
    /* Initialize the server socket */
    if (self->type == NETWORK_TYPE_NET)
        self->socket = TCPServer_init_network_socket(self->address.port);
    else if (self->type == NETWORK_TYPE_UNIX)
        self->socket = TCPServer_init_unix_socket(self->address.path);

    while (self->keep_alive) {
        /* Wait for a TCP connection */
        int client_sock = accept(self->socket, NULL, NULL);
        if (client_sock < 0) {
            if (self->keep_alive) {
                lerror("Failed to accept socket");
                lerror("ERROR [%d] %s", errno, strerror(errno));
            }
            continue;
        }

        /* Add the request to the queue */
        pthread_mutex_lock(&self->lock);
        Request* req = request_new(client_sock);
        queue_add(self->request_queue, (RefObject*) req);
        pthread_mutex_unlock(&self->lock);

        /* Notify workers of a new request */
        pthread_cond_broadcast(&self->cond);
    }
}

static PyObject* TCPServer_start(TCPServer* self, PyObject* args, PyObject* kwds) {
    self->keep_alive = 1;

    /* Start up the worker threads */
    for (U32 i = 0; i < self->worker_n; i++) {
        pthread_create(&self->worker_threads[i], NULL, (void* (*)(void*)) TCPServer_worker_run, self);
    }

    /* Start up the server main thread */
    pthread_create(&self->run_thread, NULL, (void* (*)(void*)) TCPServer_run, self);
}

static PyObject* TCPServer_stop(TCPServer* self, PyObject* args, PyObject* kwds) {
    /* Wait until we can synchronize to the worker threads */
    pthread_mutex_lock(&self->lock);
    self->keep_alive = 0;
    pthread_mutex_unlock(&self->lock);

    /* Signal the blocked worker threads */
    pthread_cond_broadcast(&self->cond);

    /* Free all thread related data */
    for (U32 i = 0; i < self->worker_n; i++) {
        pthread_join(self->worker_threads[i], NULL);
    }

    /* Tell the server to stop accepting requests */
    shutdown(self->socket, SHUT_RDWR);
    close(self->socket);

    /* Free main thread data */
    pthread_join(self->run_thread, NULL);
}

static PyObject*
TCPServer_set_request_callback(TCPServer* self, PyObject* args, PyObject* kwds) {

}

static PyObject*
TCPServer_dealloc(TCPServer* self, PyObject* args, PyObject* kwds) {
    if (self->type == NETWORK_TYPE_UNIX)
        free(self->address.path);

    pthread_mutex_destroy(&self->lock);
    free(self->worker_threads);

    OBJECT_FREE(self->request_queue);
    free(self);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyMethodDef TCPServer_methods[] = {
        {"set_request_callback", (PyCFunction) TCPServer_set_request_callback, METH_VARARGS | METH_KEYWORDS, "Set the callback function for TCP requests"},
        {"start", (PyCFunction) TCPServer_start, METH_VARARGS | METH_KEYWORDS, "Start the server"},
        {NULL}  /* Sentinel */
};

const PyTypeObject PyTCPServerType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_network.TCPServer",
        .tp_doc = "Implement a multithreaded tcp server",
        .tp_basicsize = sizeof(TCPServer),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = TCPServer_new,
        .tp_init = (initproc) TCPServer_init,
        .tp_dealloc = (destructor) TCPServer_dealloc,
        .tp_repr = (reprfunc) TCPServer_repr,
        .tp_methods = TCPServer_methods,
};
