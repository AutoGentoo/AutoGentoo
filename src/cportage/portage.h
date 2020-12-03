//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_PORTAGE_H
#define AUTOGENTOO_PORTAGE_H

#include <hacksaw/hacksaw.h>
#include <Python.h>

typedef struct Portage_prv Portage;

typedef lut_id Use_t;
typedef lut_id Package_t;

typedef enum {
    USE_OP_NONE,
    USE_OP_DISABLE, //!< !
    USE_OP_ENABLE, //!< ?
    USE_OP_LEAST_ONE, //!< ||
    USE_OP_EXACT_ONE, //!< ^^
    USE_OP_MOST_ONE, //!< ??
} use_operator_t;

typedef enum {
    USE_STATE_UNKNOWN,
    USE_STATE_DISABLED,
    USE_STATE_ENABLED
} use_state_t;

typedef enum {
    ATOM_USE_DISABLE, //!< atom[-bar]
    ATOM_USE_ENABLE, //!< atom[bar]
    ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
    ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
    ATOM_USE_EQUAL, //!< atom[bar=]
    ATOM_USE_OPPOSITE //!< atom[!bar=]
} atom_use_t;

typedef enum {
    ATOM_NO_DEFAULT, //!< use
    ATOM_DEFAULT_ON, //!< use(+)
    ATOM_DEFAULT_OFF, //!< use(-)
} atom_use_default;

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

struct Portage_prv {
    PyObject_HEAD

    LUT* global_flags;
    LUT* packages;
};

extern PyTypeObject PyPortageType;

#endif //AUTOGENTOO_PORTAGE_H
