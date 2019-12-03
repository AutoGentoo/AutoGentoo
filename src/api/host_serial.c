//
// Created by atuser on 11/25/19.
//

#include "autogentoo/api/host_serial.h"

static void PyHost_dealloc(PyHost* self) {
	free(self->id);
	free(self->hostname);
	free(self->profile);
	free(self->arch);
	
	free(self->cflags);
	free(self->cxxflags);
	free(self->use);
	
	free(self->portage_tmpdir);
	free(self->portdir);
	free(self->distdir);
	free(self->pkgdir);
	free(self->portage_logdir);
	
	PyObject_Free(self->extra);
	PyObject_Free(self);
}

static PyObject* PyHost_new(PyTypeObject* type, PyObject *args, PyObject *kwds) {
	PyHost* self = (PyHost*) type->tp_alloc(type, 0);
	self->arch = NULL;
	
	return (PyObject *)self;
}

static void PyHost_init(PyHost* self, PyObject *args, PyObject *kwds);

/* PyHost specific methods */
static PyObject* PyHost_enter_chroot(PyHost* self);