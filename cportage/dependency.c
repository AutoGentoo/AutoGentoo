//
// Created by atuser on 5/19/19.
//

#include "dependency.h"
#include "package.h"
#include "portage.h"
#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int atom_match_ebuild(Ebuild* ebuild, P_Atom* atom) {
	int cmp_slot = 0;
	int cmp_slot_sub = 0;
	
	if (ebuild->slot && atom->slot)
		cmp_slot = strcmp(ebuild->slot, atom->slot);
	if (ebuild->sub_slot && atom->sub_slot)
		cmp_slot_sub = strcmp(ebuild->sub_slot, atom->sub_slot);
	
	if (cmp_slot != 0 || cmp_slot_sub != 0)
		return 0;
	
	int cmp = 0;
	int cmp_rev = 0;
	if (atom->version) {
		cmp = atom_version_compare(ebuild->version, atom->version);
		cmp_rev = ebuild->revision - atom->revision;
	} else
		return 1;
	
	if (cmp == 0 && atom->range & ATOM_VERSION_E) {
		if (atom->range == ATOM_VERSION_E) {
			if (cmp_rev == 0)
				return 1;
		}
		else if (atom->range == ATOM_VERSION_REV)
			return 1;
		
		if (cmp_rev == 0)
			return 1;
		else if (cmp_rev > 0 && atom->range & ATOM_VERSION_L)
			return 1;
		else if (cmp_rev < 0 && atom->range & ATOM_VERSION_G)
			return 1;
	}
	
	if (cmp < 0 && atom->range & ATOM_VERSION_L) {
		return 1;
	}
	if (cmp > 0 && atom->range & ATOM_VERSION_G)
		return 1;
	
	return 0;
}


Ebuild* atom_resolve_ebuild(Emerge* emerge, P_Atom* atom) {
	Repository* repo = NULL;
	for (Repository* current = emerge->repo; current; current = current->next) {
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
	if (!target_pkg) {
		plog_warn("Package '%s' not found", atom->key);
		return NULL;
	}
	
	Ebuild* current;
	for (current = target_pkg->ebuilds; current; current = current->older) {
		if (atom_match_ebuild(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			if (!current->metadata_init)
				package_metadata_init(current);
			
			for (Keyword* keyword = target_pkg->keywords; keyword; keyword = keyword->next)
				if (atom_match_ebuild(current, keyword->atom))
					if (keyword->keywords[emerge->target_arch] < accept_keyword)
						accept_keyword = keyword->keywords[emerge->target_arch];
			if (current->keywords[emerge->target_arch] >= accept_keyword)
				return current;
		}
	}
	
	return NULL;
}

PortageDependency* dependency_new(Ebuild* e, P_Atom* p, InstalledEbuild* old, dependency_t option) {
	PortageDependency* out = malloc(sizeof(PortageDependency));
	out->target = e;
	out->selector = p;
	out->option = option;
	out->old = old;
	
	return out;
}

void dependency_resolve_ebuild(Emerge* emerge, Ebuild* ebuild) {
	if (ebuild->resolved)
		return;
	if (!ebuild->metadata_init)
		package_metadata_init(ebuild);
	if (!ebuild_check_required_use(ebuild))
		exit(1);
	
	if (emerge->build_opts == EMERGE_RUNSIDE) {
		dependency_resolve(emerge, ebuild, ebuild->rdepend);
		
		/* IM NOT REALLY SURE ABOUT THIS */
		dependency_resolve(emerge, ebuild, ebuild->pdepend);
	}
	else if (emerge->build_opts == EMERGE_BUILDSIDE) {
		dependency_resolve(emerge, ebuild, ebuild->bdepend);
		dependency_resolve(emerge, ebuild, ebuild->rdepend);
		dependency_resolve(emerge, ebuild, ebuild->depend);
		dependency_resolve(emerge, ebuild, ebuild->pdepend);
	}
	
}

void dependency_resolve(Emerge* emerge, Ebuild* current_ebuild, Dependency* depends) {
	Dependency* current;
	for (current = depends; current; current = current->next) {
		if (current->depends == HAS_DEPENDS) {
			if ((current->selector == USE_DISABLE || current->selector == USE_ENABLE)
			    && current->selector == package_check_use(current_ebuild, current->target)) {
				dependency_resolve(emerge, current_ebuild, current->selectors);
			}
			else if (current->selector == USE_LEAST_ONE) {
				int num_selected = 0;
				
				for (Dependency* num_check = current->selectors; num_check; num_check = num_check->next) {
					if (num_check->depends != IS_ATOM)
						portage_die("Invalid any of many expression in package %s-%s", current_ebuild->parent->key, current_ebuild->version);
					
					InstalledEbuild* check_installed = portagedb_resolve_installed(emerge->database, num_check->atom);
					if (check_installed) {
						num_selected++;
						continue;
					}
					
					Ebuild* check_resolved = atom_resolve_ebuild(emerge, num_check->atom);
					if (!check_resolved)
						continue;
					
					dependency_t options = 0;
					PortageDependency* check_selected = dependency_check_selected(emerge, check_resolved, NULL,
					                                                              &options);
					
					if (!check_selected)
						continue;
				}
			}
			else
				portage_die("Illegal operator %s in a dependency expression", current->target);
		}
		else {
			// ATOM
			
		}
	}
}

PortageDependency* dependency_check_selected(Emerge* emerge, Ebuild* potential, P_Atom* atom, dependency_t* options) {
	for (int i = 0; i < emerge->selected->n; i++) {
		PortageDependency* check = *(PortageDependency**)vector_get(emerge->selected, i);
		if (strcmp(potential->parent->key, check->target->parent->key) != 0)
			continue;
		
		if (strcmp(potential->slot, check->target->slot) != 0) {// Different slot
			*options = PORTAGE_SLOT;
			return NULL;
		}
		
		if (PORTAGE_INSTALL & check->option) {
			int ver_cmp = atom_version_compare(check->target->version, potential->version);
			if (ver_cmp == 0)
				*options = PORTAGE_REPLACE; //!< Already selected
			else if (ver_cmp < 0)
				*options = PORTAGE_UPDATE;
			else if (ver_cmp > 0)
				*options = PORTAGE_DOWNGRADE;
			return check;
		}
	}
	
	*options = 0;
	return NULL; //!< Package was not selected
}