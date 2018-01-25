//
// Created by atuser on 11/20/17.
//

#ifndef HACKSAW_PACKAGE_USE_H
#define HACKSAW_PACKAGE_USE_H

#include "portage.h"
#include "dependency.h"

typedef SmallMap PackageConf;   // Map category/name to PackageMeta pointer
                                // Make every package that found a loc

typedef struct __PackageMeta PackageMeta;
typedef struct __PackageUse PackageUse;
typedef struct __PackageLicense PackageLicense;
typedef PackageSelector PackageMask;
typedef PackageSelector PackageUnmask;

typedef enum {
    PM_USE,
    PM_LICENSE,
    PM_MASK,
    PM_UNMASK
} packagemeta_t;

struct __PackageUse {
    PackageSelector* selection;
    Map* use;   // key: USE_EXPAND ("" for default)
                // data: Use*
};

struct __PackageLicense {
    PackageSelector* selection;
    Vector* accepted_licenses;
};

struct __PackageMeta {
    char* key; // "category/name"
    
    // Use vectors because you can specify multiple
    // selections for the same package
    
    Vector* use;
    Vector* license;
    Vector* mask;
    Vector* unmask;
};

PackageMeta* package_meta_new ();
void package_conf_add (PackageConf* pconf, char* key, void* data, packagemeta_t dest);
void read_package_use (Portage* portage, PackageConf* dest);
void read_package_license (Portage* portage, PackageConf* dest);
void read_package_mask (Portage* portage, PackageConf* dest);
void read_package_unmask (Portage* portage, PackageConf* dest);

#endif