//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_DATABASE_H
#define AUTOGENTOO_DATABASE_H

#include <stdio.h>
#include "portage.h"
#include "package.h"
#include "directory.h"

struct __DependQuery {
	InstalledEbuild* selector;
	char* depend_atom;
};

struct __SelectedEbuild {
	Ebuild* target; //!< Package marked for installation
	
	SelectedEbuild* selected_parent;
	InstalledEbuild* installed_parent;
	Dependency* selected_by;
	
	int emerge_index; //!< We don't need all consecutive int as long at its ordered
};

struct __InstalledEbuild {
	InstalledPackage* parent;
	AtomVersion* version;
	
	char* repository;
	
	char* slot;
	char* sub_slot;
	
	Dependency* depend;
	Dependency* rdepend;
	UseFlag* use;
	
	char* cflags;
	char* cxxflags;
	char* cbuild;
	
	InstalledEbuild* older_slot;
	InstalledEbuild* newer_slot;
	
	/* Dependency backtracking */
	InstalledEbuild* next_required;
	InstalledEbuild* required_head;
};

struct __InstalledPackage {
	uint32_t hash;
	
	char* name;
	char* category;
	char* key;
	
	InstalledEbuild* installed;
};

struct __PortageDB {
	char* path; // /var/db/pkg/
	Map* installed; // KEY : category/name
};

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom);
void portagedb_add_ebuild(PortageDB* db, FPNode* cat, FPNode* pkg);
PortageDB* portagedb_read(Emerge* emerge);
void portagedb_free(PortageDB* db);
void installedebuild_free(InstalledEbuild* ebuild);
void installedpackage_free(InstalledPackage* pkg);

#endif //AUTOGENTOO_DATABASE_H
