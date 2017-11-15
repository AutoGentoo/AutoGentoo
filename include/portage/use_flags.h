//
// Created by atuser on 11/14/17.
//


#ifndef HACKSAW_USE_FLAGS_H
#define HACKSAW_USE_FLAGS_H

typedef struct __use_flag UseFlag;
typedef Vector* IUSE;

typedef enum {
    FLAG_DISABLED,
    FLAG_ENABLED
} use_t;

struct __use_flag {
    char* name;
    use_t status;
}

IUSE read_iuse (Ebuild* ebuild);
use_t iuse_get (IUSE to_check, char* flag_name);

#endif