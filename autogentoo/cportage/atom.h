//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_ATOM_H
#define AUTOGENTOO_ATOM_H

#include "Python.h"
#include "portage.h"
#include "python_util.h"

struct AtomVersion_prv {
    PyObject_HEAD
    char* full_version; //!< Only set on the first one
    char* v; //!< If there is a prefix, only include the integer part, if none 0
    atom_version_pre_t prefix;
    int revision;
    AtomVersion* next;
};

struct Atom_prv {
    PyObject_HEAD
    Package_t id;
    lut_flag_t id_flag;
    Py_hash_t cached_hash;

    char* category;
    char* name;
    char* key;
    char* repository; //!< Default is gentoo

    char* slot;
    char* sub_slot;
    atom_slot_t slot_opts;

    atom_version_t range;
    atom_block_t blocks;

    AtomVersion* version;
    AtomFlag* useflags;
};

struct AtomFlag_prv {
    PyObject_HEAD
    char* name;
    atom_use_t option;
    atom_use_default_t def;
    AtomFlag* next;
    AtomFlag* PyIterator_self__;
};

AtomFlag* atomflag_build(const char* name);
void atomflag_init(AtomFlag* self, const char* name);
PyNewFunc(PyAtom_new);
int atom_init(Atom* self, const char* input);
Atom* cmdline_atom_new(char* name);
I32 atom_version_compare(const AtomVersion* first, const AtomVersion* second);

#endif //AUTOGENTOO_ATOM_H
