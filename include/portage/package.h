//
// Created by atuser on 10/18/17.
//


#include <portage/manifest.h>

#ifndef HACKSAW_PACKAGE_H
#define HACKSAW_PACKAGE_H

#include <portage/repository.h>

typedef struct __Package Package;
typedef struct __Category Category;
typedef struct __Ebuild Ebuild;
typedef struct __EbuildVersion EbuildVersion;

struct __Category {
    char name[32];
    Vector* packages; // Package* to points in the
    Repository* repo;
};

/**
 * Package - The main wrapper for a set of ebuilds
 * The manifest
 */
struct __Package {
    Repository* repo;
    Category* category;
    Manifest* manifest;
    Vector* ebuilds; // Vector<EbuildVersion>, just create version not the entire ebuild
    
    char name[128];
};


struct __EbuildVersion {
    Vector* version;
    char* suffix;
    unsigned char revision; // 0 to disable
};

struct __Ebuild {
    Package* parent;
    Conf* metadata;
    EbuildVersion* version; // Pointer to parent->ebuilds
};

Category* category_new (Repository* repo, char* name);
Package* package_new (Repository* repo, Category* category, char* name);
void package_get_file (Package* pkg, char* filename);


#endif //HACKSAW_PACKAGE_H
