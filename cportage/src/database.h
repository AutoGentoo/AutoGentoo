//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_DATABASE_H
#define AUTOGENTOO_DATABASE_H

#include <stdio.h>
#include "portage.h"
#include "package.h"
#include "directory.h"
#include "resolve.h"

struct __InstalledEbuild {
	InstalledPackage* parent;
	AtomVersion* version;
	int revision;
	
	char* slot;
	char* sub_slot;
	
	Dependency* depend;
	Dependency* rdepend;
	UseFlag* use;
	
	Vector* rebuild_depend;
	Vector* rebuild_rdepend;
	
	/* Don't update this package if it doesn't match
	 * the installed ebuild depend dependecy requests */
	Vector* required_by;
	
	char* cflags;
	char* cxxflags;
	char* cbuild;
	
	InstalledEbuild* older_slot;
	InstalledEbuild* newer_slot;
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
	Vector* rebuilds; //!< Array of rebuild requests
	Vector* backtracking;
	
	Vector* blockers;
};

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom, char* target_slot);
void portagedb_add_ebuild(PortageDB* db, FPNode* cat, FPNode* pkg);
PortageDB* portagedb_read(Emerge* emerge);
void portagedb_free(PortageDB* db);
void installedebuild_free(InstalledEbuild* ebuild);
void installedpackage_free(InstalledPackage* pkg);
dependency_t ebuild_installedebuild_cmp(Ebuild* ebuild, InstalledEbuild* installed_ebuild);
int portagedb_ebuild_match(InstalledEbuild* ebuild, P_Atom* atom);

#endif //AUTOGENTOO_DATABASE_H
