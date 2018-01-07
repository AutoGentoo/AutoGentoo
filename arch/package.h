//
// Created by atuser on 1/5/18.
//

#ifndef AUTOGENTOO_ABS_PACKAGE_H
#define AUTOGENTOO_ABS_PACKAGE_H

#include "aabs.h"

typedef struct __aabs_pkg_t aabs_pkg_t;

typedef enum {
    AABS_PKG_REASON_WORLD = 0, //!< Pull in by world (defined in DBPATH/world)
    AABS_PKG_REASON_DEPEND = 1
} aabs_pkgreason_t;

typedef enum {
    AABS_PKG_VALIDATION_UNKNOWN = 0,
    AABS_PKG_VALIDATION_NONE = (1 << 0),
    AABS_PKG_VALIDATION_MD5SUM = (1 << 1),
    AABS_PKG_VALIDATION_SHA256SUM = (1 << 2),
    AABS_PKG_VALIDATION_SIGNATURE = (1 << 3)
} aabs_pkgvalidation_t;

struct __aabs_pkg_t {
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
    
    aabs_filelist_t files;
    
    aabs_svec_t* licenses;
    aabs_svec_t* replaces;
    aabs_svec_t* groups;
    aabs_svec_t* backup;
    aabs_vec_t*  depends;
    aabs_vec_t*  optdepends;
    aabs_vec_t*  conflicts;
    aabs_vec_t*  provides;
    
    aabs_time_t builddate;
    aabs_time_t installdate;
    
    off_t size;
    off_t isize;
    off_t download_size;
    
    aabs_pkgreason_t reason;
    aabs_pkgvalidation_t validation;
};

void aabs_pkg_init_from_desc (FILE* fp);

#endif //AUTOGENTOO_ABS_PACKAGE_H
