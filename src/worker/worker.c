//
// Created by atuser on 11/30/18.
//

#include <Python.h>

#include <autogentoo/pool.h>

#define WORKER_DEFAULT_TCP "worker.tcp"

typedef struct {
	PyObject_HEAD
	int tcp_socket;
	PoolHandler* pool_handler;
	int pool_number;
} Worker;

static void
Worker_dealloc(Worker* self) {
	pool_exit(self->pool_handler);
	close(self->tcp_socket);
	
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject*
Worker_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	Worker* self;
	
	self = (Worker*)type->tp_alloc(type, 0);
	
	if (!self)
		return NULL;
	self->pool_handler = NULL;
	self->pool_number = 0;
	self->tcp_socket = -1;
	
	return (PyObject*)self;
}

static int
Worker_init(Worker *self, PyObject *args, PyObject *kwds) {
	
	PyObject* file_location, tmp;
	
	static char *kwlist[] = {"pool_number", "file_location", NULL};
	
	if (! PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &self->pool_number, &file_location))
		return -1;
	
	char* file_loc;
	if (file_location) {
		Py_IncRef(file_location);
		file_loc = PyBytes_AsString(file_location);
		Py_DECREF(file_location);
	}
	else
		file_loc = strdup(WORKER_DEFAULT_TCP);
	
	
}
