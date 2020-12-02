//
// Created by tumbar on 12/2/20.
//

#include "expr.h"

static void atomflag_free(AtomFlag* self)
{
    OBJECT_DECREF(self->next);
    free(self->name);
    free(self);
}

AtomFlag* atomflag_build(char* name)
{
    AtomFlag* out = malloc(sizeof(AtomFlag));
    out->free = (void (*)(void*)) atomflag_free;
    out->reference_count = 0;
    out->option = ATOM_USE_ENABLE;

    if (name[0] == '-') {
        out->option = ATOM_USE_DISABLE;
        name++;
    }

    out->name = strdup(name);
    out->def = 0;
    out->next = NULL;

    return out;
}

static const struct {
    size_t prefix_string_len;
    atom_version_pre_t pre;
} atom_prefix_links[] = {
        {5, ATOM_PREFIX_ALPHA},
        {4, ATOM_PREFIX_BETA},
        {3, ATOM_PREFIX_PRE},
        {2, ATOM_PREFIX_RC},
        {0, ATOM_PREFIX_NONE},
        {1, ATOM_PREFIX_P},
};

static void atom_version_free(AtomVersion* self)
{
    if (!self)
        return;

    /* We are not reference counted so we can assume this is the head node */
    do
    {
        AtomVersion* next_node = self->next;
        if (self->v)
            free(self->v);
        free(self);

        self = next_node;
    } while (self);
}

static AtomVersion* atom_version_new_prv()
{
    AtomVersion* self = malloc(sizeof(AtomVersion));
    self->free = (void (*)(void*)) atom_version_free;
    self->v = NULL;
    self->next = NULL;
    self->prefix = 0;
    return self;
}

AtomVersion* atom_version_new(char* input) {
    char* version_str = strdup(input);
    AtomVersion* parent = NULL;
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
        if (prefix_splt == NULL) { // No prefix
            prefix_splt = buf;
        } else {
            prefix_len = prefix_splt - buf;
        }

        atom_version_pre_t prefix = -1;

        prefix = -1;
        for (int i = 0; i < sizeof(atom_prefix_links) / sizeof(atom_prefix_links[0]); i++) {
            if (prefix_len == atom_prefix_links[i].prefix_string_len) {
                prefix = atom_prefix_links[i].pre;
                break;
            }
        }
        if (prefix == -1) {
            char* ebuf = strndup(buf, prefix_len);
            lwarning("Invalid version prefix: '%s'", ebuf);
            free(ebuf);
            free(version_str);
            return NULL;
        }

        next_node = atom_version_new_prv();
        next_node->v = strdup(prefix_splt);
        next_node->prefix = prefix;

        if (!parent)
            parent = next_node;
        else
            current_node->next = next_node;
        current_node = next_node;

        if (!buf_splt)
            break;

        buf = buf_splt + 1;
        buf_splt = strpbrk(buf, "._-");
    }

    free(version_str);
    parent->full_version = strdup(input);
    return parent;
}

void atomversion_free(AtomVersion* parent) {
    AtomVersion* next = NULL;
    AtomVersion* curr = parent;
    while (curr) {
        next = curr->next;
        if (curr->full_version)
            free(curr->full_version);
        free(curr->v);
        free(curr);
        curr = next;
    }
}

static void atom_free(Atom* self)
{
    OBJECT_FREE(self->version);
    OBJECT_DECREF(self->useflags);
    free(self->key);

    if (self->slot)
        free(self->slot);
    if (self->sub_slot)
        free(self->sub_slot);

    free(self->repository);
    free(self->category);
    free(self->name);
    free(self);
}

Atom* atom_new(const char* input)
{
    Atom* out = malloc(sizeof(Atom));
    out->free = (void (*)(void*)) atom_free;
    out->revision = 0;
    out->version = NULL;
    out->useflags = NULL;
    out->slot = NULL;
    out->sub_slot = NULL;
    out->sub_opts = ATOM_SLOT_IGNORE;
    out->range = ATOM_VERSION_ALL;
    out->blocks = ATOM_BLOCK_NONE;
    out->repo_selected = ATOM_REPO_ALL;

    char* d_input = strdup(input);
    char* cat_splt = strchr(d_input, '/');
    if (!cat_splt)
    {
        lwarning("Invalid atom: %s", d_input);
        return NULL;
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
            out->revision = (int) strtol(last_dash + 2, NULL, 10);
        } else
            *last_dash = '-';
    }

    if (ver_splt)
    {
        out->version = atom_version_new(ver_splt + 1);
        *ver_splt = 0;
    }

    out->category = strdup(d_input);
    out->name = strdup(cat_splt + 1);

    // TODO Make this more robust
    out->repository = strdup("gentoo");

    asprintf(&out->key, "%s/%s", out->category, out->name);

    free(d_input);
    return out;
}

Atom* cmdline_atom_new(char* name)
{
    char* cmd_temp = NULL;
    asprintf(&cmd_temp, "SEARCH/%s", name);

    Atom* out = atom_new(cmd_temp);
    free(cmd_temp);
    free(name);

    return out;
}
