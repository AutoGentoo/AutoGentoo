//
// Created by atuser on 11/4/17.
//


#if !defined(HACKSAW_COMPILER_ATOM_H) && !defined(HACKSAW_LANGUAGE_H)
#define HACKSAW_COMPILER_ATOM_H

#include <autogentoo/hacksaw/portage/package.h>

typedef struct __package AtomSelector;

struct __package {
	char* category;
	char* name;
	EbuildVersion version;
};

AtomSelector* atom_selector_new(char* cat, char* name);

void set_atom_selector_version(EbuildVersion* v, char* version_str, int r);

Vector* parse_version(char* v_str);

void print_atom_selector(AtomSelector* pkg);

void print_ebuild_version(Vector* ver);

void free_atom_selector(AtomSelector* ptr);

#endif // HACKSAW_COMPILER_PACKAGE_H