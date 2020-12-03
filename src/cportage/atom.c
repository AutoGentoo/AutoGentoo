//
// Created by tumbar on 12/2/20.
//

#include "Python.h"
#include "atom.h"
#include "python_util.h"
#include "structmember.h"

static PyObject*
PyAtom_repr(Atom* self)
{
    PyObject* obj = PyUnicode_FromFormat(
            "Atom<category=%s, name=%s>",
            self->category,
            self->name);

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

PyNewFunc(PyAtom_new) {return type->tp_alloc(type, 0);}

static PyInitFunc(PyAtom_init, Atom)
{
    static char* kwlist[] = {"atom_string", NULL};
    const char* atom_string = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &atom_string))
        return -1;

    return atom_init(self, atom_string);
}

static PyMethod(PyAtom_dealloc, Atom)
{
    Py_XDECREF(self->useflags);
    Py_XDECREF(self->version);

    free(self->key);

    if (self->slot)
        free(self->slot);
    if (self->sub_slot)
        free(self->sub_slot);

    free(self->repository);
    free(self->category);
    free(self->name);
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


static void atomflag_free(AtomFlag* self)
{
    Py_XDECREF(self->next);
    free(self->name);
    free(self);
}

AtomFlag* atomflag_build(char* name)
{
    AtomFlag* out = malloc(sizeof(AtomFlag));
    out->option = ATOM_USE_ENABLE;

    if (name[0] == '-')
    {
        out->option = ATOM_USE_DISABLE;
        name++;
    }

    out->name = strdup(name);
    out->def = 0;
    out->next = NULL;

    return out;
}

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

static PyNewFunc(PyAtomVersion_new)
{
    AtomVersion* self = (AtomVersion*) type->tp_alloc(type, 0);
    self->v = NULL;
    self->full_version = NULL;
    self->next = NULL;
    self->prefix = 0;

    return (PyObject*) self;
}

static void atom_version_init(AtomVersion* self, const char* input)
{
    char* version_str = strdup(input);
    AtomVersion* current_node = NULL;
    AtomVersion* next_node = NULL;

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

        atom_version_pre_t prefix = -1;

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

    if (self->v)
        free(self->v);
    if (self->full_version)
        free(self->full_version);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

int atom_init(Atom* self, const char* input)
{
    if (!global_portage)
    {
        lerror("global_portage needs to be initialize with autogentoo_cportage.init()");
        return 1;
    }

    self->revision = 0;
    self->version = NULL;
    self->useflags = NULL;
    self->slot = NULL;
    self->sub_slot = NULL;
    self->slot_opts = ATOM_SLOT_IGNORE;
    self->range = ATOM_VERSION_ALL;
    self->blocks = ATOM_BLOCK_NONE;

    char* d_input = strdup(input);
    char* cat_splt = strchr(d_input, '/');
    if (!cat_splt)
    {
        lwarning("Invalid atom: %s", d_input);
        return 1;
    }

    *cat_splt = 0;
    char* name_ident = cat_splt + 1;
    char* ver_splt = NULL;

    char* last_dash = strrchr(name_ident, '-');
    int check_second = 0;

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
            self->revision = (int) strtol(last_dash + 2, NULL, 10);
        } else
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

    lut_flag_t flag = 0;
    self->id = lut_get_id(global_portage->packages, self->key, &flag);
    printf("got id %lu\n", self->id);
    if (flag == LUT_FLAG_NOT_FOUND)
    {
        /* Add the package to the LUT */
        lut_insert_id(global_portage->packages, self->key, NULL, self->id, flag);
    }

    free(d_input);
    return 0;
}

I32 atom_version_compare(AtomVersion* first, AtomVersion* second)
{
    AtomVersion* cf = first;
    AtomVersion* cs = second;

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

        I32 cmp_1 = 0;
        I32 cmp_2 = 0;

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

    return 0;
}
PyObject* PyAtom_richcompare(Atom* self, Atom* other, int op)
{
    int compare = strcmp(self->category, other->category);
    if (compare != 0)
        Py_RETURN_RICHCOMPARE(0, compare, op);

    compare = strcmp(self->name, other->name);
    Py_RETURN_RICHCOMPARE(0, compare, op);
}

PyObject* PyAtomVersion_richcompare(AtomVersion* self, AtomVersion* other, int op)
{
    int compare = atom_version_compare(self, other);
    Py_RETURN_RICHCOMPARE(0, compare, op);
}

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
        {"revision", T_INT, offsetof(Atom, revision), READONLY},
        {"version", T_OBJECT, offsetof(Atom, version), READONLY},
        {"useflags", T_OBJECT, offsetof(Atom, useflags), READONLY},
        {NULL, 0, 0, 0, NULL}
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
        .tp_richcompare = (richcmpfunc) PyAtom_richcompare,
};
