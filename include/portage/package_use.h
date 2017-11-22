//
// Created by atuser on 11/20/17.
//

#ifndef HACKSAW_PACKAGE_USE_H
#define HACKSAW_PACKAGE_USE_H

#include <tools/small_map.h>
#include <share.h>

typedef SmallMap PackageUseConf;    // Map category/name to PackageUse pointer
                                    // Make every package that found a loc 
typedef struct __PackageUse PackageUse;

struct __PackageUse {
    PackageSelector* select;
    Vector* use;
};



#endif