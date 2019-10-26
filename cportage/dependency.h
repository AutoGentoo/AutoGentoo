//
// Created by atuser on 5/19/19.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "constants.h"
#include "atom.h"

typedef enum {
	PORTAGE_NEW = 1 << 0, //!< New package
	PORTAGE_SLOT = 1 << 1, //!< Install side by side
	PORTAGE_REMOVE = 1 << 2, //!< Remove package 'old'
	PORTAGE_UPDATE = 1 << 3, //!< Install a new package and remove the old one
	PORTAGE_DOWNGRADE = 1 << 4,
	PORTAGE_REPLACE = 1 << 5,
	PORTAGE_USE_FLAG = 1 << 6,
	PORTAGE_BLOCK = 1 << 7
} dependency_t;

struct __SelectedEbuild {
	/* Next and depends not relevant */
	SelectedEbuild* parent_ebuild;
	Vector* use_change;
	
	Dependency* selected_by;
	InstalledEbuild* installed;
	Ebuild* ebuild;
	
	dependency_t action;
	
	UseFlag* useflags;
	UseFlag* explicit_flags;
};

/**
 * Resolve a top-level depedency, generated a vector of selected ebuild,
 * All blockers are added to the end
 * @param parent the emerge environment
 * @param depend expression to look at
 */
Vector* pd_layer_resolve(Emerge* parent, Dependency* depend);


/* Helpers */
Package* atom_resolve_package(Emerge* emerge, P_Atom* atom);
int pd_slot_cmp(char* slot_1, char* sub_slot_1, char* slot_2, char* sub_slot_2);
SelectedEbuild* package_resolve_ebuild(Package* pkg, P_Atom* atom);
SelectedEbuild* pd_check_selected(Vector* selected, SelectedEbuild* check);

/* Util */
void selected_ebuild_print(Emerge *em, SelectedEbuild *se);
void selected_ebuild_free(SelectedEbuild* se);

#endif //AUTOGENTOO_DEPENDENCY_H
