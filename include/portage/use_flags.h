//
// Created by atuser on 11/14/17.
//


#ifndef HACKSAW_USE_FLAGS_H
#define HACKSAW_USE_FLAGS_H

#include <language/share.h>
#include <tools/map.h>

typedef Map* IUSE;

IUSE read_iuse (Ebuild* ebuild);
use_t iuse_get (IUSE to_check, char* flag_name);
void iuse_parse (IUSE to_update, char* iuse_str);
IUSE iuse_new ();

#endif