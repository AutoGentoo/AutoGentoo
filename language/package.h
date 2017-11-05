//
// Created by atuser on 11/4/17.
//


#ifndef HACKSAW_COMPILER_PACKAGE_H
#define HACKSAW_COMPILER_PACKAGE_H

#include <tools/vector.h>

typedef struct __package PackageSelector;

struct __package {
    char* category;
    char* name;
    Vector* version;
};

PackageSelector* package_selector_new (char* cat, char* name);
Vector* parse_version (char* v_str);
void print_package_selector (PackageSelector* pkg);
void print_package_version (Vector* ver);
void free_package_selector (PackageSelector* ptr);

#endif // HACKSAW_COMPILER_PACKAGE_H