//
// Created by atuser on 11/25/19.
//

#ifndef AUTOGENTOO_HOST_SERIAL_H
#define AUTOGENTOO_HOST_SERIAL_H

#include <Python.h>
#include <structmember.h>

typedef struct {
	PyObject_HEAD
	/* Your internal 'loc' data. */
	char* id; //!< The ID of the Host
	char* profile; //!< Portage profile, see possible values with eselect profile list
	char* hostname; //!< Hostname of the host (shows up in the graphical client)
	char* arch; //!<  The portage-arch (eg. amd64)
	
	char* cflags; //!< The gcc passed to C programs, try -march=native :)
	char* cxxflags; //!< The gcc passed only to CXX programs
	char* use; //!< use flags
	
	char* portage_tmpdir; //!< build dir
	char* portdir; //!< ebuild portage tree
	char* distdir; //!< distfiles
	char* pkgdir; //!< path to binaries
	char* portage_logdir; //!< logs
	
	PyObject* extra; /* PyDict */
} PyHost;

static struct PyMemberDef PyHost_members[] = {
		{"id", T_STRING, offsetof(PyHost, id), 0, "Host id"},
		{"profile", T_STRING, offsetof(PyHost, profile), 0, "Portage profile"},
		{"hostname", T_STRING, offsetof(PyHost, hostname), 0, "Arbitrary hostname"},
		{"arch", T_STRING, offsetof(PyHost, arch), 0, "Architecture"},
		
		{"cflags", T_STRING, offsetof(PyHost, cflags), 0, "CFLAGS"},
		{"cxxflags", T_STRING, offsetof(PyHost, cxxflags), 0, "CXXFLAGS"},
		{"use", T_STRING, offsetof(PyHost, use), 0, "use flags"},
		{"portage_tmpdir", T_STRING, offsetof(PyHost, portage_tmpdir), 0, "build dir"},
		{"portdir", T_STRING, offsetof(PyHost, portdir), 0, "ebuild portage tree"},
		{"distdir", T_STRING, offsetof(PyHost, distdir), 0, "distfiles"},
		{"pkgdir", T_STRING, offsetof(PyHost, pkgdir), 0, "path to binaries"},
		{"portage_logdir", T_STRING, offsetof(PyHost, portage_logdir), 0, "logs"},
		
		{"extra", Py_TPFLAGS_DICT_SUBCLASS, offsetof(PyHost, extra), 0, "extras to environment"},
		
		{NULL}  /* Sentinel */
};

/* Python class methods */
static void PyHost_dealloc(PyHost* self);
static PyObject* PyHost_new(PyTypeObject* type, PyObject *args, PyObject *kwds);
static void PyHost_init(PyHost* self, PyObject *args, PyObject *kwds);

/* PyHost specific methods */
static PyObject* PyHost_enter_chroot(PyHost* self);

static PyMethodDef PyHost_methods[] = {
		{"enter_chroot", (PyCFunction)PyHost_enter_chroot, METH_VARARGS,"Enter a PID & BLOCK namespace to secure a chroot session",},
		{NULL}  /* Sentinel */
};


static PyTypeObject pyhost_template = {
		PyObject_HEAD_INIT(NULL)
		"host_serial.PyHost",      /*tp_name*/
		sizeof(PyHost), /*tp_basicsize*/
		0,                         /*tp_itemsize*/
		(destructor)PyHost_dealloc,/*tp_dealloc*/
		0,                         /*tp_print*/
		0,                         /*tp_getattr*/
		0,                         /*tp_setattr*/
		0,                         /*tp_compare*/
		0,                         /*tp_repr*/
		0,                         /*tp_as_number*/
		0,                         /*tp_as_sequence*/
		0,                         /*tp_as_mapping*/
		0,                         /*tp_hash */
		0,                         /*tp_call*/
		0,                         /*tp_str*/
		0,                         /*tp_getattro*/
		0,                         /*tp_setattro*/
		0,                         /*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,/*tp_flags*/
		"PyHost objects",          /* tp_doc */
		0,                         /* tp_traverse */
		0,                         /* tp_clear */
		0,                         /* tp_richcompare */
		0,                         /* tp_weaklistoffset */
		0,                         /* tp_iter */
		0,                         /* tp_iternext */
		PyHost_methods,      /* tp_methods */
		PyHost_members,      /* tp_members */
		0,                         /* tp_getset */
		0,                         /* tp_base */
		0,                         /* tp_dict */
		0,                         /* tp_descr_get */
		0,                         /* tp_descr_set */
		0,                         /* tp_dictoffset */
		(initproc)PyHost_init,/* tp_init */
		0,                         /* tp_alloc */
		PyHost_new,                 /* tp_new */
};

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
inithost_serial(void);

#endif //AUTOGENTOO_HOST_SERIAL_H
