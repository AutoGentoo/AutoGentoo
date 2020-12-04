//
// Created by tumbar on 12/3/20.
//

#ifndef AUTOGENTOO_EBUILD_H
#define AUTOGENTOO_EBUILD_H

#include <stdio.h>
#include "dependency.h"
#include "use.h"

typedef struct Ebuild_prv Ebuild;

struct Ebuild_prv {
    PyObject_HEAD
    char* name;
    char* category;
    char* repository_path;

    char* slot;
    char* sub_slot;
    char* key;
    char* package_key;

    char* ebuild;
    char* path;
    char* cache_file;

    /* Cached in the database */
    Dependency* depend;
    Dependency* bdepend;
    Dependency* rdepend;
    Dependency* pdepend; //!< Must be installed after this package is

    UseFlag* use; //!< Read iuse, then apply globals (make.conf), then package.use
    Vector* feature_restrict;
    keyword_t keywords[ARCH_END];

    RequiredUse* required_use;
    Dependency* src_uri;

    AtomVersion* version;

    U64 md5[2];         /* Checksum of the metadata file */
    U8 metadata_init;   /* Metadata has been parsed */

    Ebuild* older;
    Ebuild* newer;
};

extern PyTypeObject PyEbuildType;

#endif //AUTOGENTOO_EBUILD_H
