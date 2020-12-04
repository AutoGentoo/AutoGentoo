//
// Created by tumbar on 12/3/20.
//

#include <stdio.h>
#include "ebuild.h"
#include "language.h"
#include <structmember.h>

PyNewFunc(PyEbuild_new)
{
    Ebuild* self = (Ebuild*) type->tp_alloc(type, 0);
    memset(&self->name, 0, sizeof(Ebuild) - offsetof(Ebuild, name));
    return (PyObject*) self;
}

int ebuild_init(Ebuild* self,
                const char* repository_path,
                const char* category,
                const char* name_and_version)
{
    asprintf(&self->key, "%s/%s", category, name_and_version);
    self->repository_path = strdup(repository_path);

    Atom* atom = atom_parse(self->key);
    if (!atom)
    {
        char error_str[128];
        snprintf(error_str, 128, "Failed to initialize atom '%s'", self->key);
        PyErr_SetString(PyExc_RuntimeError, error_str);
        return -1;
    }

    /* Take all of the atom's references away */
    SAFE_REF_TAKE(self->package_key, atom->key);
    SAFE_REF_TAKE(self->version, atom->version);
    SAFE_REF_TAKE(self->name, atom->name);
    SAFE_REF_TAKE(self->category, atom->category);
    SAFE_REF_TAKE(self->slot, atom->slot);
    SAFE_REF_TAKE(self->sub_slot, atom->sub_slot);

    /* Free the atom (no longer needed) */
    Py_DECREF(atom);

    return 0;
}

PyInitFunc(PyEbuild_init, Ebuild)
{
    static char* kwlist[] = {"repository_path", "category", "name_and_version", NULL};
    const char* repository_path = NULL;
    const char* category = NULL;
    const char* name_and_version = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss", kwlist,
                                     &repository_path,
                                     &category,
                                     &name_and_version))
        return -1;
    return ebuild_init(self, repository_path, category, name_and_version);
}

PyMethod(PyEbuild_dealloc, Ebuild)
{
    SAFE_FREE(self->name);
    SAFE_FREE(self->category);
    SAFE_FREE(self->repository_path);
    SAFE_FREE(self->key);
    SAFE_FREE(self->package_key);
    SAFE_FREE(self->path);
    SAFE_FREE(self->cache_file);
    SAFE_FREE(self->slot);
    SAFE_FREE(self->sub_slot);

    Py_XDECREF(self->depend);
    Py_XDECREF(self->bdepend);
    Py_XDECREF(self->rdepend);
    Py_XDECREF(self->pdepend);

    Py_XDECREF(self->use);
    if (self->feature_restrict)
        OBJECT_FREE(self->feature_restrict);

    Py_XDECREF(self->required_use);
    Py_XDECREF(self->src_uri);

    Py_XDECREF(self->version);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyMemberDef PyEbuild_members[] = {
        {"name", T_STRING, offsetof(Ebuild, name), READONLY},
        {"category", T_STRING, offsetof(Ebuild, category), READONLY},
        {"slot", T_STRING, offsetof(Ebuild, slot), READONLY},
        {"sub_slot", T_STRING, offsetof(Ebuild, sub_slot), READONLY},
        {"package_key", T_STRING, offsetof(Ebuild, package_key), READONLY},
        {"key", T_STRING, offsetof(Ebuild, key), READONLY},
        {"ebuild", T_STRING, offsetof(Ebuild, ebuild), READONLY},
        {"path", T_STRING, offsetof(Ebuild, path), READONLY},
        {"cache_file", T_STRING, offsetof(Ebuild, cache_file), READONLY},
        {"depend", T_OBJECT, offsetof(Ebuild, depend), READONLY},
        {"bdepend", T_OBJECT, offsetof(Ebuild, bdepend), READONLY},
        {"rdepend", T_OBJECT, offsetof(Ebuild, rdepend), READONLY},
        {"pdepend", T_OBJECT, offsetof(Ebuild, pdepend), READONLY},
        {"use", T_OBJECT, offsetof(Ebuild, use), READONLY},
        {"required_use", T_OBJECT, offsetof(Ebuild, required_use), READONLY},
        {"src_uri", T_OBJECT, offsetof(Ebuild, src_uri), READONLY},
        {"version", T_OBJECT, offsetof(Ebuild, version), READONLY},
        {"metadata_init", T_BOOL, offsetof(Ebuild, metadata_init), READONLY},
        {"older", T_OBJECT, offsetof(Ebuild, older), READONLY},
        {"newer", T_OBJECT, offsetof(Ebuild, newer), READONLY},
        {NULL, 0, 0, 0, NULL}
};

PyTypeObject PyEbuildType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Ebuild",
        .tp_doc = "A package that can be emerged",
        .tp_basicsize = sizeof(Ebuild),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyEbuild_new,
        .tp_init = (initproc) PyEbuild_init,
        .tp_dealloc = (destructor) PyEbuild_dealloc,
        .tp_members = PyEbuild_members,
};
