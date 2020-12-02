//
// Created by tumbar on 12/2/20.
//

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "expr.h"

static void use_flag_free(UseFlag* self)
{
    free(self->name);
    free(self);
}

static void dependency_free(Dependency* self)
{
    OBJECT_DECREF(self->atom);
    OBJECT_DECREF(self->next);
    OBJECT_DECREF(self->children);
    free(self);
}

static Dependency* dependency_new()
{
    Dependency* self = calloc(1, sizeof(Dependency));
    self->free = (void (*)(void*)) dependency_free;
    return self;
}

static UseFlag* use_flag_new(const char* name, use_state_t state)
{
    UseFlag* self = malloc(sizeof(UseFlag));
    self->free = (void (*)(void*)) use_flag_free;
    self->reference_count = 0;
    self->name = strdup(name);
    self->state = state;

    return self;
}

static void request_use_free(RequiredUse* self)
{
    OBJECT_DECREF(self->next);
    OBJECT_DECREF(self->depend);
    free(self);
}

static Use_t use_get_global(Portage* parent, const char* useflag)
{
    if (useflag)
    {
        lut_flag_t flag = 0;
        Use_t out = lut_get_id(parent->global_flags, useflag, &flag);

        /* This use flag has not been initialized at the global state yet */
        if (flag == LUT_FLAG_NOT_FOUND)
        {
            /* Add the flag to the global map */
            lut_insert_id(parent->global_flags, useflag, out,
                          (RefObject*) use_flag_new(useflag, USE_STATE_UNKNOWN),
                          flag);
        }

        return out;
    }

    return 0;
}

RequiredUse* use_build_required_use(Portage* parent, const char* target, use_operator_t option)
{
    RequiredUse* out = malloc(sizeof(RequiredUse));
    out->free = (void (*)(void*)) request_use_free;
    out->reference_count = 0;
    out->global_flag = use_get_global(parent, target);

    out->option = option;
    out->depend = NULL;
    out->next = NULL;
    return out;
}

Dependency* dependency_build_atom(Atom* atom)
{
    Dependency* self = dependency_new();
    self->depends = IS_ATOM;

    OBJECT_INCREF(atom);
    self->atom = atom;

    return self;
}

Dependency* dependency_build_grouping(Dependency* children)
{
    Dependency* self = dependency_new();
    self->depends = HAS_DEPENDS;

    OBJECT_INCREF(children);
    self->children = children;

    return self;
}

Dependency* dependency_build_use(Portage* parent,
                                 const char* use_flag,
                                 use_operator_t type,
                                 Dependency* children)
{
    Dependency* self = dependency_new();
    self->use_condition = use_get_global(parent, use_flag);
    self->depends = HAS_DEPENDS;

    OBJECT_INCREF(children);
    self->children = children;

    return self;
}
