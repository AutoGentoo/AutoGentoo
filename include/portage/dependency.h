//
// Created by atuser on 11/5/17.
//

#ifndef HACKSAW_DEPENDENCY_H
#define HACKSAW_DEPENDENCY_H

#include <portage/package.h>

typedef struct __Dependency Dependency;
typedef struct __Package

typedef enum {
    INSTALLED = 0x0,
    NOT_INSTALED = 0x1,
    REQUIRE_REBUILD = 0x2
} depend_t;



struct __Dependency {
    Vector* dependencies; // Vector<Dependency>
    Ebuild* package;
    int status;
};

Dependency* dependency_scan (Ebuild* package);
Ebuild* get_ebuild_from_atom_select (PackageSelector* selector);

#endif // HACKSAW_DEPENDENCY_H