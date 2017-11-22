//
// Created by atuser on 11/20/17.
//

#ifndef HACKSAW_PACKAGE_USE_H
#define HACKSAW_PACKAGE_USE_H

#include <tools/small_map.h>
#include <share.h>

typedef SmallMap PackageConf;   // Map category/name to PackageUse pointer
                                // Make every package that found a loc

typedef struct __PackageUse PackageUse;
typedef struct __PackageLicense PackageLicense;
typedef PackageSelector PackageMask;
typedef PackageSelector PackageUnmask;

struct __PackageUse {
    PackageSelector* select;
    Map* use;   // key: USE_EXPAND ("" for default)
                // data: Use*
};

struct __PackageLicense {
    PackageSelector* select;
    Vector* accepted_licenses;
}

PackageUse* 

#endif