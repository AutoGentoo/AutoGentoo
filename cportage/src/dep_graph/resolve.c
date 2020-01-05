//
// Created by atuser on 12/31/19.
//

#include <errno.h>
#include "resolve.h"
#include "string.h"
#include "../portage.h"
#include "../package.h"
#include "../emerge.h"


int pd_compare_range(int cmp, atom_version_t range) {
	if ((cmp == 0 && range & ATOM_VERSION_E)
	    || (cmp > 0 && range & ATOM_VERSION_G)
	    || (cmp < 0 && range & ATOM_VERSION_L))
		return 1;
	return 0;
}

int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom) {
	if (strcmp(ebuild->parent->key, atom->key) !=0)
		return 0;
	
	int slot_cmp = 0;
	int sub_slot_cmp = 0;
	
	if (atom->slot) {
		slot_cmp = strcmp(ebuild->slot, atom->slot);
		if (ebuild->sub_slot && atom->sub_slot)
			sub_slot_cmp = strcmp(ebuild->sub_slot, atom->sub_slot);
		
		
		if (slot_cmp != 0)
			return 0;
		
		if (sub_slot_cmp != 0)
			return 0;
	}
	
	int cmp = 0;
	int cmp_rev = 0;
	if (atom->version) {
		cmp = atom_version_compare(ebuild->version, atom->version);
		cmp_rev = ebuild->revision - atom->revision;
	} else {
		return 1;
	}
	
	if (cmp == 0 && atom->range & ATOM_VERSION_E) {
		if (atom->range == ATOM_VERSION_E) {
			return cmp_rev == 0;
		} else if (atom->range == ATOM_VERSION_REV)
			return 1;
	}
	
	if (pd_compare_range(cmp, atom->range))
		return 1;
	
	return 0;
}

Package* package_resolve_atom(Emerge* em, P_Atom* atom) {
	Repository* repo = NULL;
	for (Repository* current = em->repos; current; current = current->next) {
		if (strcmp(atom->repository, current->name) == 0) {
			repo = current;
			break;
		}
	}
	
	if (!repo) {
		errno = EINVAL;
		plog_error("Repository not found %s", atom->repository);
		return NULL;
	}
	
	Package* target_pkg = map_get(repo->packages, atom->key);
	
	return target_pkg;
}