//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_EXPR_H
#define AUTOGENTOO_EXPR_H

#include "portage.h"

typedef struct Dependency_prv Dependency;
typedef struct RequiredUse_prv RequiredUse;
typedef struct Atom_prv Atom;
typedef struct AtomVersion_prv AtomVersion;
typedef struct AtomFlag_prv AtomFlag;
typedef struct UseFlag_prv UseFlag;
typedef struct Dependency_prv Dependency;

struct RequiredUse_prv {
    REFERENCE_OBJECT
    Use_t global_flag;
    use_operator_t option;
    RequiredUse* depend; /* Use this if we have a child expression for this operator */
    RequiredUse* next;
};

struct UseFlag_prv {
    REFERENCE_OBJECT
    char* name;
    use_state_t state;
};

/**
 * selector? ( child child child ) next_target? ( ... ) depend
 */
struct Dependency_prv {
    REFERENCE_OBJECT

    depend_t depends;
    use_operator_t use_operator;
    Use_t use_condition;

    Atom* atom;    // NULL if it has children

    Dependency* children;
    Dependency* next;
};

struct AtomVersion_prv {
    OBJECT_HEADER
    char* full_version; //!< Only set on the first one
    char* v; //!< If there is a prefix, only include the integer part, if none 0
    atom_version_pre_t prefix;
    AtomVersion* next;
};

struct Atom_prv {
    REFERENCE_OBJECT
    Package_t target;

    char* category;
    char* name;
    char* repository; //!< Default is gentoo
    atom_repo_t repo_selected;

    char* key;

    char* slot;
    char* sub_slot;
    atom_slot_t sub_opts;

    atom_version_t range;
    atom_block_t blocks;

    AtomVersion* version;
    int revision;

    AtomFlag* useflags;
};

struct AtomFlag_prv {
    REFERENCE_OBJECT
    char* name;
    atom_use_t option;
    atom_use_default def;
    AtomFlag* next;
};

RequiredUse* use_build_required_use(Portage* portage, const char* target, use_operator_t option);
AtomFlag* atomflag_build(char* name);

Dependency* dependency_build_use(Portage* portage, const char* use_flag, use_operator_t type, Dependency* children);
Dependency* dependency_build_grouping(Dependency* children);
Dependency* dependency_build_atom(Atom* atom);

Atom* atom_new(const char* input);
Atom* cmdline_atom_new(char* name);

#endif //AUTOGENTOO_EXPR_H
