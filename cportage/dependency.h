//
// Created by atuser on 5/19/19.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

#include "constants.h"
#include "atom.h"

typedef enum {
	PORTAGE_INSTALL = 1 << 0,
	PORTAGE_NEW = PORTAGE_INSTALL | 1 << 1, //!< New package
	PORTAGE_SLOT = PORTAGE_NEW | 1 << 2, //!< Install side by side
	PORTAGE_REMOVE = 1 << 3, //!< Remove package 'old'
	PORTAGE_UPDATE = PORTAGE_REMOVE | PORTAGE_NEW, //!< Install a new package and remove the old one
	PORTAGE_DOWNGRADE = 1 << 4 | PORTAGE_UPDATE,
	PORTAGE_REPLACE = PORTAGE_INSTALL| 1 << 5,
	PORTAGE_REBIULD = PORTAGE_REPLACE | 1 << 6,
	PORTAGE_BLOCK = 1 << 7
} dependency_t;

/**
 *              TOP
 *             /   \
 *           self  next
 *         /          \
 *    self->target    next->target
 */
struct __PortageDependency {
	Ebuild* target;
	InstalledEbuild* old;
	P_Atom* selector;
	dependency_t option;
};

Ebuild* atom_resolve_ebuild(Emerge* emerge, P_Atom* atom);
void dependency_resolve(Emerge* emerge, Ebuild* current_ebuild, Dependency* depends);
void dependency_resolve_ebuild(Emerge* emerge, Ebuild* ebuild);
PortageDependency* dependency_new(Ebuild* e, P_Atom* p, InstalledEbuild* old, dependency_t option);
PortageDependency* dependency_check_selected(Emerge* emerge, Ebuild* potential, dependency_t* options);

#endif //AUTOGENTOO_DEPENDENCY_H
