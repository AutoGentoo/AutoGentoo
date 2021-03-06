//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_PORTAGE_H
#define AUTOGENTOO_PORTAGE_H

#include "common.h"
#include <hacksaw/hacksaw.h>
#include <Python.h>
#include "python_util.h"


typedef lut_id Use_t;
typedef lut_id Package_t;

typedef enum {
    USE_OP_DISABLE, //!< !
    USE_OP_ENABLE, //!< ?
    USE_OP_LEAST_ONE, //!< ||
    USE_OP_EXACT_ONE, //!< ^^
    USE_OP_MOST_ONE, //!< ??
} use_operator_t;

enum {
    USE_STATE_DISABLED = 0,
    USE_STATE_ENABLED = 1
};

typedef enum {
    ATOM_USE_DISABLE, //!< atom[-bar]
    ATOM_USE_ENABLE, //!< atom[bar]
    ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
    ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
    ATOM_USE_EQUAL, //!< atom[bar=]
    ATOM_USE_OPPOSITE //!< atom[!bar=]
} atom_use_t;

typedef enum {
    ATOM_DEFAULT_NONE, //!< use
    ATOM_DEFAULT_ON, //!< use(+)
    ATOM_DEFAULT_OFF, //!< use(-)
} atom_use_default_t;

typedef enum {
    ATOM_VERSION_NONE,
    ATOM_VERSION_E = 0x1,
    ATOM_VERSION_L = 0x2, //!< <app-misc/foo-1.23	A version strictly before 1.23 is required.
    ATOM_VERSION_G = 0x4, //!< >app-misc/foo-1.23	A version strictly later than 1.23 is required.
    ATOM_VERSION_REV = ATOM_VERSION_E | 0x8,  //!< ~app-misc/foo-1.23	Version 1.23 (or any 1.23-r*) is required.

    ATOM_VERSION_ALL = ATOM_VERSION_E | ATOM_VERSION_L | ATOM_VERSION_G,//!< app-misc/foo-1.23		Any version
    ATOM_VERSION_GE = ATOM_VERSION_E | ATOM_VERSION_G, //!< >=app-misc/foo-1.23	Version 1.23 or later is required.
    ATOM_VERSION_LE = ATOM_VERSION_L | ATOM_VERSION_E, //!< <=app-misc/foo-1.23	Version 1.23 or older is required.
} atom_version_t;

typedef enum {
    ATOM_BLOCK_NONE,
    ATOM_BLOCK_SOFT,
    ATOM_BLOCK_HARD
} atom_block_t;

typedef enum {
    ATOM_SLOT_IGNORE,
    ATOM_SLOT_REBUILD
} atom_slot_t;

typedef enum {
    ATOM_PREFIX_ALPHA,
    ATOM_PREFIX_BETA,
    ATOM_PREFIX_PRE,
    ATOM_PREFIX_RC,
    ATOM_PREFIX_NONE,
    ATOM_PREFIX_P
} atom_version_pre_t;

/**
 * Only includes supported architectures
 * amd64	x86	alpha	arm	arm64	hppa	ia64	ppc	ppc64	sparc
 */
typedef enum {
    ARCH_AMD64,
    ARCH_X86,
    ARCH_ARM,
    ARCH_ARM64,
    ARCH_HPPA,
    ARCH_IA64,
    ARCH_PPC,
    ARCH_PPC64,
    ARCH_SPARC,
    ARCH_END
} arch_t;

typedef enum {
    KEYWORD_BROKEN,
    KEYWORD_NONE,
    KEYWORD_UNSTABLE,
    KEYWORD_STABLE,
} keyword_t;

struct Portage_prv {
    PyObject_HEAD

    LUT* global_flags;
    LUT* packages;
};

PyNewFunc(PyPortage_new);
PyInitFunc(PyPortage_init, Portage);
PyFastMethod(PyPortage_match_atom, Portage);
PyFastMethod(PyPortage_add_package, Portage);
PyFastMethod(PyPortage_get_package, Portage);

#endif //AUTOGENTOO_PORTAGE_H
