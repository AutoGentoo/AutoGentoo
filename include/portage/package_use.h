//
// Created by atuser on 11/20/17.
//

#ifndef HACKSAW_PACKAGE_USE_H
#define HACKSAW_PACKAGE_USE_H

#include <tools/map.h>
#include <share.h>

typedef struct __PackageUseConf PackageUseConf;
typedef struct __PackageUse PackageUse;

struct __PackageUseConf {
    Map* packages;  // Dont need map for efficiency, only for easy data access
            // Means that threshold can be low.
};

struct __PackageUse {
    PackageSelector* select;
    Vector* use;
};



#endif