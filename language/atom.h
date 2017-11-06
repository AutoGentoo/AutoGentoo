//
// Created by atuser on 11/4/17.
//


#ifndef HACKSAW_COMPILER_PACKAGE_H
#define HACKSAW_COMPILER_PACKAGE_H

#include <tools/vector.h>

typedef struct __package AtomSelector;

#include <portage/package.h>

struct __package {
    char* category;
    char* name;
    EbuildVersion version;
};

AtomSelector* atom_selector_new (char* cat, char* name);
void set_atom_selector_version (EbuildVersion* v, char* version_str, int r);
Vector* parse_version (char* v_str);
void print_atom_selector (AtomSelector* pkg);
void print_ebuild_version (Vector* ver);
void free_atom_selector (AtomSelector* ptr);

#endif // HACKSAW_COMPILER_PACKAGE_H