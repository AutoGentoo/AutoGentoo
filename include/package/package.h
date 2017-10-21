//
// Created by atuser on 10/18/17.
//

#ifndef HACKSAW_PACKAGE_H
#define HACKSAW_PACKAGE_H

#include <package/manifest.h>

typedef struct __Package Package;
typedef struct __Category Category;

struct __Category {
    char name[32];
    Vector* packages; // Package* to points in the
};

/**
 * Package - The main wrapper for a set of ebuilds
 * The manifest
 */
struct __Package {
    char category[64];
    char name[128];

    Manifest manifest;
};

Package* package_new (char* category, char* name);


#endif //HACKSAW_PACKAGE_H
