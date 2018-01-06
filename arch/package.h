//
// Created by atuser on 1/5/18.
//

#ifndef AUTOGENTOO_ABS_PACKAGE_H
#define AUTOGENTOO_ABS_PACKAGE_H

#include "aabs.h"

typedef struct __aabs_package_t aabs_package_t;

typedef enum {
    AABS_REASON_WORLD = 0, //!< Pull in by world (defined in DBPATH/world)
    AABS_REASON_DEPEND = 1
} aabs_pkg_reason_t;

struct __aabs_package_t {
    unsigned long name_hash;
    char* filename;
    char* base;
    char* name;
    char* version;
    char* desc;
    char* url;
    char* packager;
    char* md5sum;
    char* sha256sum;
    char* base64_sig;
    char* arch;
    
    /* The binary archive */
    struct archive* mtree;
    
    aabs_node_t* licenses;
    aabs_node_t* replaces;
    aabs_node_t* groups;
    aabs_node_t* backup;
    aabs_node_t* depends;
    aabs_node_t* optdepends;
    aabs_node_t* conflicts;
    aabs_node_t* provides;
    
    aabs_filelist_t files;
};


#endif //AUTOGENTOO_ABS_PACKAGE_H
