//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_DATABASE_H
#define AUTOGENTOO_DATABASE_H

#include <stdio.h>
#include "portage.h"
#include "package.h"
#include "directory.h"
#include "dependency.h"

struct __DependQuery {
	InstalledEbuild* selector;
	char* depend_atom;
};

typedef enum {
	EBUILD_REBUILD_DEPEND = 0x1, //!< Part of DEPEND set
	EBUILD_REBUILD_RDEPEND = 0x2 //!< Part of RDEPEND set
} rebuild_t;

struct __RebuildEbuild {
	InstalledEbuild* rebuild;
	InstalledEbuild* old_slot;
	P_Atom* selector;
	Ebuild* new_slot; //!< NULL until the installation
	rebuild_t type;
};

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
	Vector* backtracking; //!< Array of rebuild requests
};

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom, char* target_slot);
void portagedb_add_ebuild(PortageDB* db, FPNode* cat, FPNode* pkg);
PortageDB* portagedb_read(Emerge* emerge);
void portagedb_free(PortageDB* db);
void installedebuild_free(InstalledEbuild* ebuild);
void installedpackage_free(InstalledPackage* pkg);
void backtrack_search(PortageDB* db, InstalledEbuild* parent, Dependency* deptree, rebuild_t type);
void backtrack_new(PortageDB* db, InstalledEbuild* rebuild, P_Atom* atom, rebuild_t type);
void backtrack_resolve(PortageDB* db, rebuild_t types);
dependency_t ebuild_installedebuild_cmp(Ebuild* ebuild, InstalledEbuild* installed_ebuild);

#endif //AUTOGENTOO_DATABASE_H
