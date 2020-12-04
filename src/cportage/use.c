//
// Created by tumbar on 12/2/20.
//

#include <string.h>
#include "use.h"
#include "python_util.h"
#include "language.h"
#include <structmember.h>

static PyNewFunc(PyUseFlag_new)
{
    UseFlag* self = (UseFlag*) type->tp_alloc(type, 0);
    self->name = NULL;
    self->state = 0;
    return (PyObject*) self;
}

static void use_flag_init(UseFlag* self, const char* name, use_state_t state)
{
    self->name = strdup(name);
    self->state = state;
}

static PyInitFunc(PyUseFlag_init, UseFlag)
{
    static char* kwlist[] = {"name", "state", NULL};

    const char* name = NULL;
    use_state_t state = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sp", kwlist, &name, &state))
        return -1;

    self->state = state;
    self->name = strdup(name);

    return 0;
}

static PyMethod(PyUseFlag_dealloc, UseFlag)
{
    SAFE_FREE(self->name);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

Use_t use_get_global(Portage* parent, const char* useflag)
{
    if (useflag)
    {
        lut_flag_t flag = 0;
        Use_t out = lut_get_id(parent->global_flags, useflag, &flag);

        /* This use flag has not been initialized at the global state yet */
        if (flag == LUT_FLAG_NOT_FOUND)
        {
            UseFlag* new_flag = (UseFlag*) PyUseFlag_new(&PyUseFlagType, NULL, NULL);
            use_flag_init(new_flag, useflag, USE_STATE_UNKNOWN);

            /* Add the flag to the global map */
            lut_insert_id(parent->global_flags, useflag, ref_pyobject((PyObject*) new_flag),
                          out, flag);

            /* reference held by ref_pyobject */
            Py_DECREF(new_flag);
        }

        return out;
    }

    return 0;
}

static PyNewFunc(PyRequiredUse_new)
{
    RequiredUse* self = (RequiredUse*) type->tp_alloc(type, 0);
    self->depend = NULL;
    self->next = NULL;
    self->global_flag = 0;
    self->option = 0;
    return (PyObject*) self;
}

static PyMethod(PyRequiredUse_dealloc, RequiredUse)
{
    Py_XDECREF(self->next);
    Py_XDECREF(self->depend);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyInitFunc(PyRequiredUse_init, RequiredUse)
{
    static char* kwlist[] = {"required_use_string", NULL};
    const char* required_use_string = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &required_use_string))
        return -1;

    RequiredUse* duped = required_use_parse(required_use_string);
    if (!duped)
        return -1;

    memcpy(&self->global_flag, &duped->global_flag, sizeof(RequiredUse) - offsetof(RequiredUse, global_flag));
    Py_TYPE(duped)->tp_free((PyObject*) duped);
    return 0;
}

RequiredUse* use_build_required_use(const char* target, use_operator_t option)
{
    RequiredUse* out = (RequiredUse*) PyRequiredUse_new(&PyRequiredUseType, NULL, NULL);
    out->global_flag = use_get_global(global_portage, target);

    out->option = option;
    out->depend = NULL;
    out->next = NULL;
    return out;
}

static PyMemberDef PyUseFlag_members[] = {
        {"name", T_STRING, offsetof(UseFlag, name), READONLY},
        {"state", T_BOOL, offsetof(UseFlag, state), 0},
        {NULL }
};


static PyMemberDef PyRequiredUse_members[] = {
        {"depend", T_OBJECT, offsetof(RequiredUse, depend), READONLY},
        {"next", T_OBJECT, offsetof(RequiredUse, next), READONLY},
        {"id", T_ULONGLONG, offsetof(RequiredUse, global_flag), READONLY},
        {"operator", T_INT, offsetof(RequiredUse, option), READONLY},
        {NULL }
};

PyTypeObject PyUseFlagType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.UseFlag",
        .tp_doc = "A way to keep track of the state of a use flag",
        .tp_basicsize = sizeof(UseFlag),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyUseFlag_new,
        .tp_init = (initproc) PyUseFlag_init,
        .tp_dealloc = (destructor) PyUseFlag_dealloc,
        .tp_members = PyUseFlag_members,
};

PyTypeObject PyRequiredUseType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "autogentoo_cportage.RequiredUse",
        .tp_doc = "Model required use syntax",
        .tp_basicsize = sizeof(RequiredUse),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyRequiredUse_new,
        .tp_init = (initproc) PyRequiredUse_init,
        .tp_dealloc = (destructor) PyRequiredUse_dealloc,
        .tp_members = PyRequiredUse_members
};
