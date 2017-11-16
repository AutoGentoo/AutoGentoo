//
// Created by atuser on 11/14/17.
//


#ifndef HACKSAW_USE_FLAGS_H
#define HACKSAW_USE_FLAGS_H

#include <language/share.h>

typedef Vector* IUSE;

IUSE read_iuse (Ebuild* ebuild);
use_t iuse_get (IUSE to_check, char* flag_name);

IUSE iuse_new ();

#endif