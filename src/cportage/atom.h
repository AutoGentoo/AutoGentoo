//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_ATOM_H
#define AUTOGENTOO_ATOM_H

#include "portage.h"
#include "python_util.h"

typedef struct Atom_prv Atom;
typedef struct AtomVersion_prv AtomVersion;
typedef struct AtomFlag_prv AtomFlag;

struct AtomVersion_prv {
    PyObject_HEAD
    char* full_version; //!< Only set on the first one
    char* v; //!< If there is a prefix, only include the integer part, if none 0
    atom_version_pre_t prefix;
    AtomVersion* next;
};

struct Atom_prv {
    PyObject_HEAD
    Package_t id;

    char* category;
    char* name;
    char* key;
    char* repository; //!< Default is gentoo

    char* slot;
    char* sub_slot;
    atom_slot_t slot_opts;

    atom_version_t range;
    atom_block_t blocks;
    int revision;

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
I32 atom_version_compare(AtomVersion* first, AtomVersion* second);

extern PyTypeObject PyAtomFlagType;
extern PyTypeObject PyAtomVersionType;
extern PyTypeObject PyAtomType;

#endif //AUTOGENTOO_ATOM_H
