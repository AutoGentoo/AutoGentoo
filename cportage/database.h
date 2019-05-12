//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_DATABASE_H
#define AUTOGENTOO_DATABASE_H

#include <stdio.h>
#include "portage.h"
#include "package.h"

struct __InstalledEbuild {
	InstalledPackage* parent;
	AtomVersion* version;
	
	char* repository;
	
	char* slot;
	char* sub_slot;
	atom_slot_t sub_opts;
	
	P_Atom* depend;
	P_Atom* rdepend;
	UseFlag* use;
	
	char* cflags;
	char* cxxflags;
	char* cbuild;
	
	InstalledEbuild* next;
};

struct __InstalledPackage {
	char* name;
	char* category;
	char* key;
	
	InstalledEbuild* installed;
};

struct __PortageDB {
	char* path; // /var/db/pkg/
	Map* installed; // KEY : category/name
};

PortageDB* portagedb_read(Emerge* emerge);

#endif //AUTOGENTOO_DATABASE_H
