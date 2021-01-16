//
// Created by tumbar on 12/3/20.
//

#include <stdio.h>
#include "ebuild.h"
#include "package.h"
#include "language.h"
#include <structmember.h>

PyNewFunc(PyEbuild_new)
{
    Ebuild* self = (Ebuild*) type->tp_alloc(type, 0);
    memset(&self->name, 0, sizeof(Ebuild) - offsetof(Ebuild, name));
    return (PyObject*) self;
}

int ebuild_init(Ebuild* self,
                const char* ebuild_repo_path,
                const char* cache_repo_path,
                const char* category,
                const char* name_and_version)
{
    asprintf(&self->key, "%s/%s", category, name_and_version);

    Atom* atom = (Atom*) PyAtom_new(&PyAtomType, NULL, NULL);
    atom_init(atom, self->key);

    if (!atom)
    {
        PyErr_Format(PyExc_RuntimeError, "Failed to parse atom '%s'", self->key);
        return -1;
    }

    self->package_id = atom->id;
    self->package_lut_flags = atom->id_flag;
    /* Take all of the atom's references away */
    SAFE_REF_TAKE(self->package_key, atom->key)
    SAFE_REF_TAKE(self->version, atom->version)
    SAFE_REF_TAKE(self->name, atom->name)
    SAFE_REF_TAKE(self->category, atom->category)
    SAFE_REF_TAKE(self->slot, atom->slot)
    SAFE_REF_TAKE(self->sub_slot, atom->sub_slot)

    Py_XDECREF(self->iuse);
    self->iuse = PyDict_New();

    /* Free the atom (no longer needed) */
    Py_DECREF(atom);

    assert(self->version);

    char cache_file_raw[PATH_MAX];
    if (ebuild_repo_path)
    {
        snprintf(cache_file_raw,  PATH_MAX,"%s/%s/%s-%s.ebuild",
                ebuild_repo_path, self->package_key, self->name,
                self->version->full_version);

        self->ebuild = realpath(cache_file_raw, NULL);
        if (!self->ebuild)
        {
            PyErr_Format(PyExc_FileNotFoundError, "Ebuild file not found '%s'", cache_file_raw);
            return -1;
        }
    }
    else
    {
        self->ebuild = NULL;
    }

    snprintf(cache_file_raw, PATH_MAX,"%s/%s", cache_repo_path, self->key);
    self->cache_file = realpath(cache_file_raw, NULL);
    if (!self->cache_file)
    {
        PyErr_Format(PyExc_FileNotFoundError, "Cache file not found '%s'", cache_file_raw);
        return -1;
    }

    return 0;
}

static void ebuild_keyword_init(Ebuild* self, char* line)
{
    const static struct
    {
        arch_t l;
        char* str;
    } keyword_links[] = {
            {ARCH_AMD64, "amd64"},
            {ARCH_X86,   "x86"},
            {ARCH_ARM,   "arm"},
            {ARCH_ARM64, "arm64"},
            {ARCH_HPPA,  "hppa"},
            {ARCH_IA64,  "ia64"},
            {ARCH_PPC,   "ppc"},
            {ARCH_PPC64, "ppc64"},
            {ARCH_SPARC, "sparc"},
            {ARCH_END, NULL}
    };

    for (char* tok = strtok(line, " \n\t"); tok; tok = strtok(NULL, " \n\t"))
    {
        if (tok[0] == 0)
            continue;
        keyword_t opt = KEYWORD_STABLE;
        size_t offset = 0;
        if (tok[0] == '~')
        {
            opt = KEYWORD_UNSTABLE;
            offset = 1;
        } else if (tok[0] == '-')
        {
            opt = KEYWORD_BROKEN;
            offset = 1;
        }

        char* target = tok + offset;
        if (strcmp(target, "**") == 0)
        {
            for (int i = 0; i < ARCH_END; i++)
                self->keywords[i] = opt;
            continue;
        }
        if (strcmp(target, "*") == 0)
        {
            for (int i = 0; i < ARCH_END; i++)
                self->keywords[i] = opt;
            continue;
        }

        for (int i = 0; i < ARCH_END; i++)
        {
            if (strcmp(keyword_links[i].str, target) == 0)
            {
                self->keywords[keyword_links[i].l] = opt;
                break;
            }
        }
    }
}

static void ebuild_iuse_init(Ebuild* self, char* line)
{
    for (char* token = strtok(line, " "); token; token = strtok(NULL, " "))
    {
        UseFlag* flag = (UseFlag*) PyUseFlag_new(&PyUseFlagType, NULL, NULL);

        if (*token == '+')
        {
            flag->state = USE_STATE_ENABLED;
            token++;
        } else if (*token == '-')
        {
            flag->state = USE_STATE_DISABLED;
            token++;
        }

        flag->name = strdup(token);

        PyDict_SetItemString(self->iuse, flag->name, (PyObject*) flag);
        Py_DECREF(flag);
    }
}

int ebuild_metadata_init(Ebuild* self)
{
    FILE* fp = fopen(self->cache_file, "r");
    if (!fp)
    {
        PyErr_Format(PyExc_FileNotFoundError, "Failed to open %s", self->cache_file);
        return -1;
    }

    for (int i = 0; i < ARCH_END; i++)
        self->keywords[i] = KEYWORD_NONE;

    static const struct
    {
        const char* name_match;
        U64 ebuild_target_offset;
    } depend_setup[] = {
            {"DEPEND",  offsetof(Ebuild, depend)},
            {"RDEPEND", offsetof(Ebuild, rdepend)},
            {"PDEPEND", offsetof(Ebuild, pdepend)},
            {"BDEPEND", offsetof(Ebuild, bdepend)},
    };

    char* name = NULL, * value = NULL;
    size_t name_size_n = 0, value_size_n = 0;

    U32 line_n = 0;
    while (!feof(fp))
    {
        line_n++;
        ssize_t name_size = getdelim(&name, &name_size_n, '=', fp);
        ssize_t value_size = getdelim(&value, &value_size_n, '\n', fp);

        if (!name || name_size == -1 || !value || value_size == -1)
        {
            errno = 0;
            break;
        }

        if (name_size > 0)
            name[name_size - 1] = 0;
        if (value_size)
            value[value_size - 1] = 0;

        U8 is_dep = 0;
        for (U32 i = 0; i < sizeof(depend_setup) / sizeof(depend_setup[0]); i++)
        {
            if (strcmp(name, depend_setup[i].name_match) == 0)
            {
                Dependency** target = (Dependency**) (((U8*) self) + depend_setup[i].ebuild_target_offset);
                if (*target)
                {
                    lerror("%s cannot be defined twice in the same file", name);
                    return 1;
                }

                *target = depend_parse(value);

                if (!*target)
                {
                    lerror("Failed to parse '%s' in %s", depend_setup[i].name_match, self->key);
                    return 2;
                }

                is_dep = 1;
                break;
            }
        }

        if (is_dep)
            continue;

        if (strcmp(name, "SLOT") == 0)
        {
            char* tok = strtok(value, "/");
            self->slot = strdup(tok);

            tok = strtok(NULL, "/");
            if (tok)
                self->sub_slot = strdup(tok);
        } else if (strcmp(name, "REQUIRED_USE") == 0)
        {
            self->required_use = required_use_parse(value);
            if (!self->required_use)
            {
                lerror("Failed to parse 'REQUIRED_USE' in %s", self->key);
                return 3;
            }
        }
        else if (strcmp(name, "KEYWORDS") == 0)
            ebuild_keyword_init(self, value);
        else if (strcmp(name, "IUSE") == 0)
            ebuild_iuse_init(self, value);
    }

    free(value);
    free(name);

    self->metadata_init = 1;
    if (fclose(fp) != 0)
    {
        lerror("Failed to close file '%s'", self->cache_file);
        return 4;
    }

    return 0;
}

static PyFastMethod(PyEbuild_metadata_init, Ebuild)
{
    if (nargs != 0)
    {
        PyErr_Format(PyExc_TypeError, "Expected no arguments to Ebuild.metadata_init, got '%d'", nargs);
        return NULL;
    }

    if (ebuild_metadata_init(self) != 0)
    {
        PyErr_Format(PyExc_RuntimeError, "Failed to initialize metadata for %s", self->key);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyInitFunc(PyEbuild_init, Ebuild)
{
    static char* kwlist[] = {"ebuild_repo", "cache_repo", "category", "name_and_version", NULL};
    const char* ebuild_repo = NULL;
    const char* cache_repo = NULL;
    const char* category = NULL;
    const char* name_and_version = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "zsss", kwlist,
                                     &ebuild_repo,
                                     &cache_repo,
                                     &category,
                                     &name_and_version))
        return -1;
    return ebuild_init(self, ebuild_repo, cache_repo, category, name_and_version);
}

PyDealloc(PyEbuild_dealloc, Ebuild)
{
    Py_XDECREF(self->older);

    SAFE_FREE(self->name);
    SAFE_FREE(self->category);
    SAFE_FREE(self->key);
    SAFE_FREE(self->package_key);
    SAFE_FREE(self->path);
    SAFE_FREE(self->cache_file);
    SAFE_FREE(self->ebuild);
    SAFE_FREE(self->slot);
    SAFE_FREE(self->sub_slot);

    Py_XDECREF(self->depend);
    Py_XDECREF(self->bdepend);
    Py_XDECREF(self->rdepend);
    Py_XDECREF(self->pdepend);

    Py_XDECREF(self->iuse);
    //OBJECT_FREE(self->feature_restrict);

    Py_XDECREF(self->required_use);
    Py_XDECREF(self->src_uri);

    Py_XDECREF(self->version);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* PyEbuild_next(Ebuild* self)
{
    Py_XINCREF(self->older);
    return (PyObject*) self->older;
}


static PyMethodDef PyEbuild_methods[] = {
        {"initialize_metadata", (PyCFunction) PyEbuild_metadata_init, METH_FASTCALL},
        {NULL, NULL, 0, NULL}        /* Sentinel */
};


static PyMemberDef PyEbuild_members[] = {
        {"name",          T_STRING, offsetof(Ebuild, name),          READONLY},
        {"category",      T_STRING, offsetof(Ebuild, category),      READONLY},
        {"slot",          T_STRING, offsetof(Ebuild, slot),          READONLY},
        {"sub_slot",      T_STRING, offsetof(Ebuild, sub_slot),      READONLY},
        {"package_key",   T_STRING, offsetof(Ebuild, package_key),   READONLY},
        {"key",           T_STRING, offsetof(Ebuild, key),           READONLY},
        {"ebuild",        T_STRING, offsetof(Ebuild, ebuild),        READONLY},
        {"path",          T_STRING, offsetof(Ebuild, path),          READONLY},
        {"cache_file",    T_STRING, offsetof(Ebuild, cache_file),    READONLY},
        {"depend",        T_OBJECT, offsetof(Ebuild, depend),        READONLY},
        {"bdepend",       T_OBJECT, offsetof(Ebuild, bdepend),       READONLY},
        {"rdepend",       T_OBJECT, offsetof(Ebuild, rdepend),       READONLY},
        {"pdepend",       T_OBJECT, offsetof(Ebuild, pdepend),       READONLY},
        {"required_use",  T_OBJECT, offsetof(Ebuild, required_use),  READONLY},
        {"src_uri",       T_OBJECT, offsetof(Ebuild, src_uri),       READONLY},
        {"version",       T_OBJECT, offsetof(Ebuild, version),       READONLY},
        {"iuse",          T_OBJECT, offsetof(Ebuild, iuse),          READONLY},
        {"metadata_init", T_BOOL,   offsetof(Ebuild, metadata_init), READONLY},
        {"package",       T_OBJECT, offsetof(Ebuild, package),       READONLY},
        {"older",         T_OBJECT, offsetof(Ebuild, older),         READONLY},
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
        .tp_iternext = (iternextfunc) PyEbuild_next,
        .tp_members = PyEbuild_members,
        .tp_methods = PyEbuild_methods,
};
