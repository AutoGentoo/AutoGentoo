//
// Created by atuser on 11/4/17.
//


#ifndef HACKSAW_COMPILER_PACKAGE_H
#define HACKSAW_COMPILER_PACKAGE_H

#include <tools/vector.h>

typedef struct __package PackageSelector;

#include <portage/package.h>

struct __package {
    char* category;
    char* name;
    EbuildVersion version;
};

PackageSelector* package_selector_new (char* cat, char* name);
void set_package_selector_version (EbuildVersion* v, char* version_str, int r);
Vector* parse_version (char* v_str);
void print_package_selector (PackageSelector* pkg);
void print_package_version (Vector* ver);
void free_package_selector (PackageSelector* ptr);

#endif // HACKSAW_COMPILER_PACKAGE_H