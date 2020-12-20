//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "atom.h"
#include "python_util.h"
#include "structmember.h"
#include "language.h"
#include "ebuild.h"

static Py_hash_t PyAtom_hash(Atom* self);

static PyObject*
PyAtom_repr(Atom* self)
{
    char prefix[3] = {0, 0, 0};

    if (self->range == ATOM_VERSION_ALL) {}
    else if (self->range & ATOM_VERSION_G)
        prefix[0] = '>';
    else if (self->range & ATOM_VERSION_L)
        prefix[0] = '<';
    else if (self->range & ATOM_VERSION_E)
        prefix[1] = '=';

    PyObject* obj;
    if (self->version)
    {
        obj = PyUnicode_FromFormat(
                "%s%s/%s-%s",
                prefix,
                self->category,
                self->name,
                self->version->full_version
        );
    }
    else
    {
        obj = PyUnicode_FromFormat(
                "%s%s/%s",
                prefix,
                self->category,
                self->name
        );
    }

    Py_XINCREF(obj);
    return obj;
}

static PyObject*
PyAtomVersion_repr(AtomVersion* self)
{
    PyObject* obj = PyUnicode_FromFormat("AtomVersion<%s>", self->full_version);
    Py_XINCREF(obj);
    return obj;
}

static PyObject*
PyAtomFlag_repr(AtomFlag* self)
{
    char prefix[2] = "\0\0";
    char suffix[2] = "\0\0";

    switch (self->option)
    {
        case ATOM_USE_DISABLE:
            *prefix = '-';
            break;
        case ATOM_USE_ENABLE:
            break;
        case ATOM_USE_ENABLE_IF_ON:
            *suffix = '?';
            break;
        case ATOM_USE_DISABLE_IF_OFF:
            *prefix = '!';
            *suffix = '?';
            break;
        case ATOM_USE_EQUAL:
            *suffix = '=';
            break;
        case ATOM_USE_OPPOSITE:
            *prefix = '!';
            *suffix = '=';
            break;
    }

    PyObject* obj = PyUnicode_FromFormat("AtomFlag<%s%s%s>", prefix, self->name, suffix);
    Py_XINCREF(obj);
    return obj;
}

PyNewFunc(PyAtom_new) {return type->tp_alloc(type, 0);}

static PyInitFunc(PyAtom_init, Atom)
{
    static char* kwlist[] = {"atom_string", NULL};
    const char* atom_string = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &atom_string))
        return -1;

    Atom* duped = atom_parse(atom_string);
    if (!duped)
        return -1;

    memcpy(&self->id, &duped->id, sizeof(Atom) - offsetof(Atom, id));
    Py_TYPE(duped)->tp_free((PyObject*) duped);
    return 0;
}

static PyMethod(PyAtom_dealloc, Atom)
{
    Py_XDECREF(self->useflags);
    Py_XDECREF(self->version);

    SAFE_FREE(self->key);
    SAFE_FREE(self->slot);
    SAFE_FREE(self->sub_slot);
    SAFE_FREE(self->repository);
    SAFE_FREE(self->category);
    SAFE_FREE(self->name);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

Atom* cmdline_atom_new(char* name)
{
    char* cmd_temp = NULL;
    asprintf(&cmd_temp, "SEARCH/%s", name);

    Atom* out = (Atom*) PyAtom_new(&PyAtomType, NULL, NULL);
    if (atom_init(out, name) != 0)
    {
        Py_DECREF(out);
        out = NULL;
    }

    free(cmd_temp);
    free(name);
    return out;
}

static PyNewFunc(PyAtomFlag_new)
{
    AtomFlag* self = (AtomFlag*) type->tp_alloc(type, 0);
    self->def = 0;
    self->name = NULL;
    self->option = 0;
    self->next = NULL;
    self->PyIterator_self__ = NULL;
    return (PyObject*) self;
}

static PyInitFunc(PyAtomFlag_init, AtomFlag)
{
    static char* kwlist[] = {"expr", NULL};
    const char* expr = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &expr))
        return -1;

    atomflag_init(self, expr);
    return 0;
}

static PyMethod(PyAtomFlag_dealloc, AtomFlag)
{
    Py_XDECREF(self->next);
    SAFE_FREE(self->name);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* PyAtomFlag_iter(AtomFlag* self)
{
    Py_INCREF(self);
    self->PyIterator_self__ = self;
    return (PyObject*) self;
}
static PyObject* PyAtomFlag_next(AtomFlag* self)
{
    if (self->PyIterator_self__)
    {
        AtomFlag* out = self->PyIterator_self__;
        Py_INCREF(out);
        self->PyIterator_self__ = self->PyIterator_self__->next;
        return (PyObject*) out;
    }

    return NULL;
}

AtomFlag* atomflag_build(const char* name)
{
    AtomFlag* self = (AtomFlag*) PyAtomFlag_new(&PyAtomFlagType, NULL, NULL);
    atomflag_init(self, name);
    return self;
}

void atomflag_init(AtomFlag* self, const char* name)
{
    self->option = ATOM_USE_ENABLE;

    if (name[0] == '-')
    {
        self->option = ATOM_USE_DISABLE;
        name++;
    }

    self->name = strdup(name);
    self->def = 0;
    self->next = NULL;
}

static PyNewFunc(PyAtomVersion_new)
{
    AtomVersion* self = (AtomVersion*) type->tp_alloc(type, 0);
    self->v = NULL;
    self->full_version = NULL;
    self->next = NULL;
    self->prefix = 0;
    self->revision = 0;

    return (PyObject*) self;
}

static void atom_version_init(AtomVersion* self, const char* input)
{
    static const struct
    {
        U8 prefix_string_len;
        atom_version_pre_t pre;
    } atom_prefix_links[] = {
            {5, ATOM_PREFIX_ALPHA},
            {4, ATOM_PREFIX_BETA},
            {3, ATOM_PREFIX_PRE},
            {2, ATOM_PREFIX_RC},
            {0, ATOM_PREFIX_NONE},
            {1, ATOM_PREFIX_P},
    };

    char* version_str = strdup(input);
    AtomVersion* current_node = NULL;
    AtomVersion* next_node = NULL;

    char* last_dash = strrchr(version_str, '-');
    if (last_dash && last_dash[1] == 'r')
    {
        self->revision = (int) strtol(last_dash + 2, NULL, 10);
        *last_dash = 0;
    }

    char* buf = version_str;
    char* buf_splt = strpbrk(buf, "._-");

    while (1)
    {
        if (buf_splt)
            *buf_splt = 0;

        char* prefix_splt = strpbrk(buf, "0123456789");
        size_t prefix_len = 0;
        if (prefix_splt == NULL) // No prefix
            prefix_splt = buf;
        else
            prefix_len = prefix_splt - buf;

        atom_version_pre_t prefix;
        prefix = -1;
        for (int i = 0; i < sizeof(atom_prefix_links) / sizeof(atom_prefix_links[0]); i++)
        {
            if (prefix_len == atom_prefix_links[i].prefix_string_len)
            {
                prefix = atom_prefix_links[i].pre;
                break;
            }
        }

        if (prefix == -1)
        {
            char* ebuf = strndup(buf, prefix_len);
            lwarning("Invalid version prefix: '%s'", ebuf);
            free(ebuf);
            free(version_str);
            return;
        }

        if (!current_node)
        {
            self->next = NULL;
            self->v = strdup(prefix_splt);
            self->prefix = prefix;
            next_node = self;
        }
        else
        {
            next_node = (AtomVersion*) PyAtomVersion_new(&PyAtomVersionType, NULL, NULL);
            next_node->v = strdup(prefix_splt);
            next_node->prefix = prefix;
            current_node->next = next_node;
        }
        current_node = next_node;

        if (!buf_splt)
            break;

        buf = buf_splt + 1;
        buf_splt = strpbrk(buf, "._-");
    }

    free(version_str);
    self->full_version = strdup(input);
}

static PyInitFunc(PyAtomVersion_init, AtomVersion)
{
    static char* kwlist[] = {"version_string", NULL};
    const char* version_string = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &version_string))
        return -1;

    atom_version_init(self, version_string);
    return 0;
}

PyMethod(PyAtomVersion_dealloc, AtomVersion)
{
    Py_XDECREF(self->next);
    SAFE_FREE(self->v);
    SAFE_FREE(self->full_version);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

int atom_init(Atom* self, const char* input)
{
    if (!global_portage)
    {
        lerror("global_portage needs to be initialize with autogentoo_cportage.init()");
        return 1;
    }

    self->version = NULL;
    self->useflags = NULL;
    self->slot = NULL;
    self->sub_slot = NULL;
    self->slot_opts = ATOM_SLOT_IGNORE;
    self->range = ATOM_VERSION_ALL;
    self->blocks = ATOM_BLOCK_NONE;
    self->cached_hash = -1; /* Invalidate the cache to regenerate */

    char* d_input = strdup(input);
    char* cat_splt = strchr(d_input, '/');
    if (!cat_splt)
    {
        language_print_error("Invalid atom: '%s'", d_input);
        free(d_input);
        return 1;
    }

    *cat_splt = 0;
    char* name_ident = cat_splt + 1;
    char* ver_splt = NULL;

    char* last_dash = strrchr(name_ident, '-');
    int check_second = 0;

    /* Check if this atom has a revision at the end of its version */
    if (last_dash)
    {
        if (isdigit(last_dash[1]))
            ver_splt = last_dash;
        if (last_dash[1] == 'r' && isdigit(last_dash[2]))
            check_second = 1;
    }
    if (check_second)
    {
        *last_dash = 0;
        char* second_dash = strrchr(name_ident, '-');
        if (second_dash && isdigit(second_dash[1]))
        {
            *second_dash = 0;
            ver_splt = second_dash;

        }

        /* Let version_init parse the revision */
        *last_dash = '-';
    }

    if (ver_splt)
    {
        self->version = (AtomVersion*) PyAtomVersion_new(&PyAtomVersionType, NULL, NULL);
        atom_version_init(self->version, ver_splt + 1);
        *ver_splt = 0;
    }

    self->category = strdup(d_input);
    self->name = strdup(cat_splt + 1);

    self->repository = NULL;
    asprintf(&self->key, "%s/%s", self->category, self->name);

    self->id = lut_get_id(global_portage->packages, self->key, &self->id_flag);

    free(d_input);

    self->cached_hash = PyAtom_hash(self);

    return 0;
}

I32 atom_version_compare(const AtomVersion* first, const AtomVersion* second)
{
    const AtomVersion* cf = first;
    const AtomVersion* cs = second;

    for (; cf && cs; cf = cf->next, cs = cs->next)
    {
        char* scf = cf->v;
        char* scs = cs->v;

        if (cf->prefix != cs->prefix)
            return (I32) (cf->prefix - cs->prefix);

        size_t scf_l = strlen(scf);
        size_t scs_l = strlen(scs);

        int star_skip = 0;
        if (scf[scf_l - 1] == '*')
        {
            star_skip = 1;
            scf_l--;
        } else if (scs[scs_l - 1] == '*')
        {
            star_skip = 1;
            scs_l--;
        }

        char scf_suf = 0;
        char scs_suf = 0;

        /* 12.5.4 > 12.5b */
        if (cs->next && scf[scf_l - 1] > '9')
            return -1;
        if (cf->next && scs[scs_l - 1] > '9')
            return 1;

        if (scf[scf_l - 1] > '9')
            scf_suf = scf[scf_l - 1];
        if (scs[scs_l - 1] > '9')
            scs_suf = scs[scs_l - 1];

        I32 cmp_1;
        I32 cmp_2;
        if ((scf[0] != '0' || scf_l == 1) && (scs[0] != '0' || scs_l == 1))
        {
            cmp_1 = (I32) strtol(scf, NULL, 10);
            cmp_2 = (I32) strtol(scs, NULL, 10);
        } else
        {
            I32 maxlength = scf_l > scs_l ? (I32) scf_l : (I32) scs_l;

            assert(maxlength < 128);
            static char r1[128];
            static char r2[128];
            r1[maxlength] = 0;
            r2[maxlength] = 0;

            strncpy(r1, scf, scf_l);
            strncpy(r2, scs, scs_l);

            for (int i1 = scf_l; i1 < maxlength; i1++)
                r1[i1] = '0';
            for (int i2 = scs_l; i2 < maxlength; i2++)
                r2[i2] = '0';

            cmp_1 = (I32) strtol(r1, NULL, 10);
            cmp_2 = (I32) strtol(r2, NULL, 10);
        }

        I32 cmp = cmp_1 - cmp_2;
        I32 cmp_suf = scf_suf - scs_suf;
        if (cmp != 0)
            return cmp;

        if (star_skip)
            return 0;

        if (cmp_suf != 0)
            return cmp_suf;
    }

    if (cf)
        return 1;
    if (cs)
        return -1;

    return first->revision - second->revision;
}
PyObject* PyAtom_richcompare(Atom* self, Atom* other, int op)
{
    int compare = strcmp(self->key, other->key);
    Py_RETURN_RICHCOMPARE(compare, 0, op);
}

PyObject* PyAtomVersion_richcompare(AtomVersion* self, AtomVersion* other, int op)
{
    int compare = atom_version_compare(self, other);
    Py_RETURN_RICHCOMPARE(compare, 0, op);
}

int atom_matches(Atom* self, Ebuild* target)
{
    if (strcmp(self->key, target->package_key) != 0)
    {
        return 0;
    }

    int version_compare = atom_version_compare(self->version, target->version);
    if (version_compare == 0)
        return (int)(self->range & ATOM_VERSION_E);
    else if (version_compare > 0)                     /* atom version is greater than ebuild's */
        return (int)(self->range & ATOM_VERSION_L);
    else if (version_compare < 0)
        return (int)(self->range & ATOM_VERSION_G);
}

static PyObject* PyAtom_matches(Atom* self, const PyObject* args[], const int nargs)
{
    if (nargs != 1 || !PyObject_TypeCheck(args[0], &PyEbuildType))
    {
        PyErr_SetString(PyExc_TypeError, "Expected a single argument of type Ebuild");
        return NULL;
    }

    if (atom_matches(self, (Ebuild*) args[0]))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}


static Py_hash_t PyAtom_hash(Atom* self)
{
#define PyATOM_STRING_HASH(str) str ? map_get_hash(str, strlen(str)) : 0

    if (self->cached_hash != -1)
        return self->cached_hash;

    U64 flag_hash = 0;
    for (AtomFlag* flag = self->useflags; flag; flag = flag->next)
    {
        flag_hash += map_get_hash(flag->name, strlen(flag->name));
    }

    struct {
        U64 key_hash;
        U64 version_hash;
        U64 repo_hash;
        U64 slot_hash;
        U64 sub_slot_hash;
        U64 flag_hash;
        U32 slot_opts;
        U32 range;
        U32 blocks;
    } hash_data = {
            self->id,
            self->version ? map_get_hash(self->version->full_version, strlen(self->version->full_version)) : 0,
            PyATOM_STRING_HASH(self->repository),
            PyATOM_STRING_HASH(self->slot),
            PyATOM_STRING_HASH(self->sub_slot),
            flag_hash,
            self->slot_opts,
            self->range,
            self->blocks,
    };

    return map_get_hash(&hash_data, 8*6 + 4*3);
}

static PyMethodDef PyAtom_methods[] = {
        {"matches", (PyCFunction) PyAtom_matches, METH_FASTCALL},
        {NULL, NULL, 0}
};

static PyMemberDef PyAtomFlag_members[] = {
        {"name", T_STRING, offsetof(AtomFlag, name), READONLY, "Flag name"},
        {"option", T_INT, offsetof(AtomFlag, option), READONLY, "Enable or disable?"},
        {"default", T_INT, offsetof(AtomFlag, option), READONLY, "Default value if not found"},
        {"next", T_OBJECT, offsetof(AtomFlag, next), READONLY, "Next flag in linked list"},
        {NULL, 0, 0, 0, NULL}
};

static PyMemberDef PyAtomVersion_members[] = {
        {"raw", T_STRING, offsetof(AtomVersion, full_version), READONLY, "The string representation of this version"},
        {NULL, 0, 0, 0, NULL}
};

static PyMemberDef PyAtom_members[] = {
        {"id", T_ULONGLONG, offsetof(Atom, id), READONLY, "Package id used for LUT"},
        {"category", T_STRING, offsetof(Atom, category), READONLY, "Portage package category"},
        {"name", T_STRING, offsetof(Atom, name), READONLY, "Portage package name"},
        {"key", T_STRING, offsetof(Atom, key), READONLY, "category/name"},
        {"repository", T_STRING, offsetof(Atom, repository), READONLY, "Portage package repo"},
        {"slot", T_STRING, offsetof(Atom, slot), READONLY},
        {"sub_slot", T_STRING, offsetof(Atom, sub_slot), READONLY},
        {"slot_opts", T_INT, offsetof(Atom, slot_opts), READONLY},
        {"range", T_INT, offsetof(Atom, range), READONLY},
        {"blocks", T_INT, offsetof(Atom, blocks), READONLY},
        {"version", T_OBJECT, offsetof(Atom, version), READONLY},
        {"useflags", T_OBJECT, offsetof(Atom, useflags), READONLY},
        {NULL, 0, 0, 0, NULL}
};

PyTypeObject PyAtomFlagType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.AtomFlag",
        .tp_doc = "A way to use flag switches inside an atom",
        .tp_basicsize = sizeof(AtomFlag),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyAtomFlag_new,
        .tp_init = (initproc) PyAtomFlag_init,
        .tp_dealloc = (destructor) PyAtomFlag_dealloc,
        .tp_repr = (reprfunc) PyAtomFlag_repr,
        .tp_members = PyAtomFlag_members,
        .tp_iter = (getiterfunc) PyAtomFlag_iter,
        .tp_iternext = (iternextfunc) PyAtomFlag_next
};

PyTypeObject PyAtomVersionType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.AtomVersion",
        .tp_doc = "A way to represent versions inside an Atom",
        .tp_basicsize = sizeof(AtomVersion),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyAtomVersion_new,
        .tp_init = (initproc) PyAtomVersion_init,
        .tp_dealloc = (destructor) PyAtomVersion_dealloc,
        .tp_repr = (reprfunc) PyAtomVersion_repr,
        .tp_members = PyAtomVersion_members,
        .tp_richcompare = (richcmpfunc) PyAtomVersion_richcompare
};

PyTypeObject PyAtomType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.Atom",
        .tp_doc = "A way to select a package or set of packages",
        .tp_basicsize = sizeof(Atom),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyAtom_new,
        .tp_init = (initproc) PyAtom_init,
        .tp_dealloc = (destructor) PyAtom_dealloc,
        .tp_repr = (reprfunc) PyAtom_repr,
        .tp_members = PyAtom_members,
        .tp_methods = PyAtom_methods,
        .tp_richcompare = (richcmpfunc) PyAtom_richcompare,
        .tp_hash = (hashfunc) PyAtom_hash
};
