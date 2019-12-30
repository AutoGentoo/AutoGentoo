//
// Created by atuser on 5/19/19.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "../constants.h"
#include "../atom.h"

struct __SelectedEbuild {
	/* Next and depends not relevant */
	SelectedEbuild* parent_ebuild;
	Vector* use_change;
	
	Dependency* selected_by;
	InstalledEbuild* installed;
	Ebuild* ebuild;
	
	//dependency_t action;
	
	UseFlag* useflags;
	UseFlag* explicit_flags;
};

/**
 * Resolve a top-level depedency, generated a vector of selected ebuild,
 * All blockers are added to the end
 * @param parent the emerge environment
 * @param depend expression to look at
 */
//Vector* pd_layer_resolve(Emerge* parent, Dependency* depend);

/**
 * When dependency resolution reaches a single atom
 * Resolve that atom and generate a SelectedEbuild
 * @param emerge the parent emerge environment
 * @param parent_ebuild ebuild that is selecting this one
 * @param dep dependency that triggered this selection
 * @param selected list of already selected ebuilds
 * @return The new ebuild that has been selected, if NULL, a previously selected ebuild was updated
 */
SelectedEbuild* pd_resolve_single(Emerge* emerge, SelectedEbuild* parent_ebuild, Dependency* dep, Vector* selected);

/* Helpers */
//Package* atom_resolve_package(Emerge* emerge, P_Atom* atom);
int pd_slot_cmp(char* slot_1, char* sub_slot_1, char* slot_2, char* sub_slot_2);
//SelectedEbuild* package_resolve_ebuild(Package* pkg, P_Atom* atom);
//SelectedEbuild* pd_check_selected(Vector* selected, SelectedEbuild* check);

/* Util */
void selected_ebuild_print(Emerge *em, SelectedEbuild *se);
void selected_ebuild_free(SelectedEbuild* se);

#endif //AUTOGENTOO_DEPENDENCY_H
