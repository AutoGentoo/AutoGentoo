//
// Created by atuser on 5/19/19.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

#include "constants.h"
#include "atom.h"

typedef enum {
	PORTAGE_INSTALL = 1 << 0, //!< Installing not removing
	PORTAGE_NEW = PORTAGE_INSTALL | 1 << 1, //!< New package
	PORTAGE_SLOT = PORTAGE_NEW | 1 << 2, //!< Install side by side
	PORTAGE_REMOVE = 1 << 3, //!< Remove package 'old'
	PORTAGE_UPDATE = PORTAGE_REMOVE | PORTAGE_NEW, //!< Install a new package and remove the old one
	PORTAGE_DOWNGRADE = 1 << 4 | PORTAGE_UPDATE,
	PORTAGE_REPLACE = PORTAGE_INSTALL | 1 << 5,
	PORTAGE_REBUILD = PORTAGE_REPLACE | 1 << 6,
	PORTAGE_USE_FLAG = 1 << 7 | PORTAGE_REBUILD,
	PORTAGE_BLOCK = 1 << 8
} dependency_t;

typedef enum {
	PORTAGE_DEPEND_STATUS_NOT_ADDED,
	PORTAGE_DEPEND_STATUS_ADDED
} portage_depend_st_t;

struct __PortageDependency {
	Dependency* selected_by;
	
	Ebuild* target;
	InstalledEbuild* old;
	UseFlag* flags;
	P_Atom* selector;
	dependency_t option;
	
	portage_depend_st_t status;
};

int atom_match_ebuild(Ebuild* ebuild, P_Atom* atom);
Ebuild* atom_resolve_ebuild(Emerge* emerge, P_Atom* atom);
PortageDependency* dependency_resolve_single(Emerge* emerge, Ebuild* current_ebuild, Dependency* depend, int try_keyword);
void dependency_resolve_ebuild(Emerge* emerge, Ebuild* ebuild);
PortageDependency* dependency_new(Dependency* parent, Ebuild* e, P_Atom* p, InstalledEbuild* old, dependency_t option);
PortageDependency* dependency_get_selected(Emerge* emerge, P_Atom* search);
PortageDependency* dependency_set_selected(Emerge* emerge, Ebuild* potential, P_Atom* potential_atom);

#endif //AUTOGENTOO_DEPENDENCY_H
