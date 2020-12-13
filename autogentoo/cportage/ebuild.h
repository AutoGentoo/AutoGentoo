//
// Created by tumbar on 12/3/20.
//

#ifndef AUTOGENTOO_EBUILD_H
#define AUTOGENTOO_EBUILD_H

#include <stdio.h>
#include "Python.h"
#include "common.h"
#include "portage.h"


struct Ebuild_prv {
    PyObject_HEAD
    char* name;
    char* category;

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

    PyObject* iuse;
    //Vector* feature_restrict;
    keyword_t keywords[ARCH_END];

    RequiredUse* required_use;
    Dependency* src_uri;

    AtomVersion* version;

    U64 md5[2];         /* Checksum of the metadata file */
    U8 metadata_init;   /* Metadata has been parsed */

    Package* package;
    Ebuild* older;
    Ebuild* newer;
};

PyNewFunc(PyEbuild_new);
int ebuild_init(Ebuild* self,
                const char* ebuild_repo_path,
                const char* cache_repo_path,
                const char* category,
                const char* name_and_version);

int ebuild_metadata_init(Ebuild* self);


#endif //AUTOGENTOO_EBUILD_H
