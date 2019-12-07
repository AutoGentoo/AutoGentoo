//
// Created by atuser on 12/6/19.
//

#include <stdlib.h>
#include <errno.h>
#include "resolve.h"
#include "package.h"
#include "dep_v4.h"
#include "database.h"
#include <string.h>

int pd_compare_range(int cmp, atom_version_t range) {
	if ((cmp == 0 && range & ATOM_VERSION_E)
		|| (cmp > 0 && range & ATOM_VERSION_G)
		|| (cmp < 0 && range & ATOM_VERSION_L))
		return 1;
	return 0;
}

int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom) {
	int slot_cmp = 0;
	int sub_slot_cmp = 0;
	
	if (atom->slot) {
		slot_cmp = strcmp(ebuild->slot, atom->slot);
		if (ebuild->sub_slot && atom->sub_slot)
			sub_slot_cmp = strcmp(ebuild->sub_slot, atom->sub_slot);
		
		/* Only compare the slots */
		if (!pd_compare_range(slot_cmp, atom->range))
			return 0;
		if (!pd_compare_range(sub_slot_cmp, atom->range))
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

ResolvedEbuild* resolved_ebuild_new(Ebuild* ebuild, P_Atom* atom) {
	ResolvedEbuild* temp = malloc(sizeof(ResolvedEbuild));
	temp->next = NULL;
	temp->selected_by = NULL; /* Set this in out area */
	temp->installed = portagedb_resolve_installed(ebuild->parent->parent->parent->database, atom, ebuild->slot);
	
	temp->useflags = NULL;
	temp->explicit_flags = NULL;
	
	temp->action = PORTAGE_NEW;
	temp->unstable_keywords = 0;
	temp->resolve_index = 0;
	
	if (temp->installed)
		temp->action = ebuild_installedebuild_cmp(temp->ebuild, temp->installed);
	
	return temp;
}

Package* package_resolve_atom(ResolveEmerge* emerge, P_Atom* atom) {
	Repository* repo = NULL;
	for (Repository* current = emerge->parent->repos; current; current = current->next) {
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

ResolvedEbuild* resolved_ebuild_resolve(ResolveEmerge* em, P_Atom* atom) {
	Package* pkg = package_resolve_atom(em, atom);
	if (!pkg) {
		portage_die("Package '%s' not found", atom->key);
		return NULL;
	}
	
	ResolvedEbuild* out = NULL;
	ResolvedEbuild* current_link = NULL;
	
	ResolvedEbuild* add_to_end = NULL;
	ResolvedEbuild* current_end = NULL;
	
	Ebuild* current;
	for (current = pkg->ebuilds; current; current = current->older) {
		package_metadata_init(current);
		if (ebuild_match_atom(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			
			/* Apply package.accept_keywords */
			for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next) {
				if (ebuild_match_atom(current, keyword->atom))
					if (keyword->keywords[pkg->parent->parent->target_arch] < accept_keyword)
						accept_keyword = keyword->keywords[pkg->parent->parent->target_arch];
			}
			
			ResolvedEbuild* temp = resolved_ebuild_new(current, atom);
			
			/* This is the index that this package is going to be added to */
			temp->resolve_index = em->resolved_ebuilds->n;
			
			if (current->keywords[pkg->parent->parent->target_arch] >= accept_keyword) {
				/* Preferably use this set of ebuilds, this is added before
				 * the non-accepted ebuilds
				 * */
				if (!current_link) {
					out = temp;
					current_link = temp;
				}
				else {
					current_link->next = temp;
					current_link = temp;
				}
			}
			else {
				temp->unstable_keywords = 1;
				if (!add_to_end) {
					add_to_end = temp;
					current_end = temp;
				}
				else {
					current_end->next = temp;
					current_end = temp;
				}
			}
		}
	}
	
	if (!out)
		out = add_to_end;
	else
		current_link->next = add_to_end;
	
	return out;
}