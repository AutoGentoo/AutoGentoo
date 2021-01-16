//
// Created by tumbar on 12/2/20.
//

#include <dirent.h>
#include "portage.h"
#include "python_util.h"
#include "package.h"
#include "atom.h"
#include "ebuild.h"

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

static
PyDealloc(PyPortage_dealloc, Portage)
{
    OBJECT_FREE(self->global_flags);
    OBJECT_FREE(self->packages);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

PyFastMethod(PyPortage_get_package, Portage)
{
    if (nargs != 1)
    {
        PyErr_Format(PyExc_TypeError, "Portage.get_package expects a single argument");
        return NULL;
    }

    U64 package_id = PyLong_AsUnsignedLongLong(args[0]);
    Package* package = (Package*)lut_get(global_portage->packages, package_id);

    if (!package)
        Py_RETURN_NONE;

    Py_INCREF(package);
    return (PyObject*) package;
}

PyFastMethod(PyPortage_add_package, Portage)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyPackageType))
    {
        PyErr_Format(PyExc_TypeError, "Portage.get_package expects a Package");
        return NULL;
    }

    Package* pkg = (Package*) args[0];
    pkg->package_id = lut_insert(self->packages, pkg->key, (U64) pkg, LUT_FLAG_PYTHON);

    Py_RETURN_NONE;
}

PyFastMethod(PyPortage_match_atom, Portage)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyAtomType))
    {
        PyErr_Format(PyExc_TypeError, "Portage.match_atom expects an Atom");
        return NULL;
    }

    Atom* atom = (Atom*) args[0];
    Package* pkg  = (Package*) lut_get(self->packages, atom->id);
    if (!pkg)
    {
        PyErr_Format(PyExc_KeyError, "Failed to find package with key '%s' (0x%x)", atom->key, atom->id);
        return NULL;
    }

    return PyPackage_match_atom(pkg, args, nargs);
}

PyFastMethod(PyPortage_initialize_repository, Portage)
{
    if (nargs != 2)
    {
        PyErr_Format(PyExc_TypeError, "Portage.initialize_repository expects 2 arguments");
        return NULL;
    }

    const char* ebuild_path = NULL;
    if (args[0] != Py_None)
        ebuild_path = PyUnicode_AsUTF8(args[0]);
    const char* cache_path = PyUnicode_AsUTF8(args[1]);

    struct dirent* dp;
    struct dirent* dp2;
    DIR* dfd;
    DIR* dfd2;

    if ((dfd = opendir(cache_path)) == NULL)
    {
        PyErr_Format(PyExc_RuntimeError, "Failed to open cache directory '%s'", cache_path);
        return NULL;
    }

    char path1[PATH_MAX];

    U32 i = 0;
    while ((dp = readdir(dfd)) != NULL)
    {
        struct stat stbuf;
        sprintf(path1, "%s/%s", cache_path, dp->d_name);
        if (stat(path1, &stbuf) == -1)
        {
            PyErr_Format(PyExc_RuntimeError, "Failed to stat '%s'", path1);
            closedir(dfd);
            return NULL;
        }

        if (*dp->d_name == '.')
            continue;

        if (S_ISDIR(stbuf.st_mode))
        {
            const char* category = dp->d_name;
            if ((dfd2 = opendir(path1)) == NULL)
            {
                PyErr_Format(PyExc_RuntimeError, "Failed to open cache directory '%s'", path1);
                closedir(dfd);
                return NULL;
            }

            while ((dp2 = readdir(dfd2)) != NULL)
            {
                if (*dp2->d_name == '.' || strcmp(dp2->d_name, "Manifest.gz") == 0)
                    continue;

                const char* name_and_value = dp2->d_name;

                Ebuild* ebuild = (Ebuild*) PyEbuild_new(&PyEbuildType,
                                                        NULL,
                                                        NULL);

                if (ebuild_init(ebuild, ebuild_path, cache_path,
                                category, name_and_value) != 0)
                {
                    PyErr_Format(PyExc_RuntimeError, "Failed to initialize ebuild for '%s'", name_and_value);
                    Py_DECREF(ebuild);
                    closedir(dfd2);
                    return NULL;
                }

                Package* pkg;
                if (ebuild->package_lut_flags & LUT_FLAG_NOT_FOUND)
                {
                    pkg = (Package*) PyPackage_new(&PyPackageType, NULL, NULL);

                    char* cat_split = strchr(ebuild->package_key, '/');
                    pkg->package_id = ebuild->package_id;
                    pkg->key = strdup(ebuild->package_key);
                    pkg->category = strndup(ebuild->package_key, cat_split - ebuild->package_key);
                    pkg->name = strdup(cat_split + 1);
                    pkg->ebuilds = NULL;

                    lut_insert_id(self->packages, pkg->key, (U64)pkg,
                                  pkg->package_id, LUT_FLAG_NOT_FOUND | LUT_FLAG_PYTHON);
                    Py_DECREF(pkg); // Held by lut
                }
                else
                {
                    pkg = (Package*) lut_get(self->packages, ebuild->package_id);
                    if (!pkg)
                    {
                        PyErr_Format(PyExc_SystemError, "Error while parsing metadata at '%s/%s'", category, name_and_value);
                        return NULL;
                    }
                }

                PyPackage_add_ebuild(pkg, (PyObject* const*) &ebuild, 1);
                Py_DECREF(ebuild);  // Held by package
                i++;
            }

            closedir(dfd2);
        }
    }

    closedir(dfd);

    return PyLong_FromLong(i);
}

static
PyFastMethod(PyPortage_get_use_flag, Portage)
{
    if (nargs != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Portage.get_use_flag expects one argument");
        return NULL;
    }

    lut_id id = PyLong_AsUnsignedLongLong(args[0]);
    PyObject* use_flag = (PyObject*) lut_get(self->global_flags, id);
    if (!use_flag)
    {
        Py_RETURN_NONE;
    }

    Py_INCREF(use_flag);
    return use_flag;
}


static PyMethodDef PyPortage_methods[] = {
        {"get_package", (PyCFunction) PyPortage_get_package, METH_FASTCALL, "Find a package in this instance"},
        {"add_package", (PyCFunction) PyPortage_add_package, METH_FASTCALL, "Register a package"},
        {"match_atom", (PyCFunction) PyPortage_match_atom, METH_FASTCALL},
        {"get_use_flag", (PyCFunction) PyPortage_get_use_flag, METH_FASTCALL},
        {"initialize_repository", (PyCFunction) PyPortage_initialize_repository, METH_FASTCALL},
        {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyTypeObject PyPortageType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Portage",
        .tp_doc = "General struct to manage portage data",
        .tp_basicsize = sizeof(Portage),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyPortage_new,
        .tp_methods = PyPortage_methods,
        .tp_init = (initproc) PyPortage_init,
        .tp_dealloc = (destructor) PyPortage_dealloc,
};