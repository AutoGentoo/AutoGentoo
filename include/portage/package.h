//
// Created by atuser on 10/18/17.
//


#include <portage/manifest.h>

#ifndef HACKSAW_PACKAGE_H
#define HACKSAW_PACKAGE_H

#include <portage/repository.h>

typedef struct __Package Package;
typedef struct __Category Category;

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
    char name[128];
};

Category* category_new (Repository* repo, char* name);
Package* package_new (Repository* repo, Category* category, char* name);
void package_get_file (Package* pkg, char* filename);


#endif //HACKSAW_PACKAGE_H
