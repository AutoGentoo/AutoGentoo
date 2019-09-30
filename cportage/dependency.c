//
// Created by atuser on 5/19/19.
//

#include "dependency.h"
#include "package.h"
#include "portage.h"
#include "database.h"
#include "suggestion.h"
#include <string.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include <autogentoo/hacksaw/set.h>
#include <errno.h>

int pd_slot_cmp(char* slot_1, char* sub_slot_1, char* slot_2, char* sub_slot_2) {
	if (!slot_1 && !slot_2)
		return 0;
	else if (slot_1 && !slot_2)
		return 1;
	else if (!slot_1)
		return 2;
	
	if (strcmp(slot_1, slot_2) != 0)
		return -1;
	
	if (!sub_slot_1 && !sub_slot_2)
		return 0;
	else if (sub_slot_1 && !sub_slot_2)
		return 1;
	else if (!sub_slot_1)
		return 2;
	
	if (strcmp(sub_slot_1, sub_slot_2) != 0)
		return -2;
	
	return 0;
}

/*
PortageDependency* pd_find_atm(Emerge* parent, P_Atom* atom) {
	for (int i = 0; i < parent->selected->n; i++) {
		PortageDependency* current = *(PortageDependency**)vector_get(parent->selected, i);
		
		if (strcmp(current->ebuild->parent->key, atom->key) == 0) {
			int slot_cmp = pd_slot_cmp(current->slot, current->sub_slot, atom->slot, atom->sub_slot);
			
			* Slots don't match *
			if (slot_cmp < 0)
				continue;
			
			if (slot_cmp == 2) {
				if (current->slot)
					free(current->slot);
				if (current->sub_slot)
					free(current->sub_slot);
				
				current->slot = strdup(atom->slot);
				current->sub_slot = strdup(atom->sub_slot);
			}
			
			return current;
		}
	}
	
	return NULL;
}
*/

void __pd_layer_resolve__(Emerge* parent, Dependency* depend, Ebuild* target, Vector* ebuild_set) {
	for (Dependency* current_depend = depend; current_depend; current_depend = current_depend->next) {
		if (current_depend->depends == HAS_DEPENDS) {
			/* Do logical evaluation for use flags */
			if ((depend->selector == USE_DISABLE || depend->selector == USE_ENABLE)) {
				if (depend->selector == ebuild_check_use(target, depend->target))
					__pd_layer_resolve__(parent, current_depend->selectors, target, ebuild_set);
			}
			else if (depend->selector == USE_LEAST_ONE) {
			
			}
			else
				portage_die("Illegal operator %s in a dependency expression", depend->target);
			continue;
		}
		
		
		/* Resolve the ebuild */ /*
		Package* pkg = atom_resolve_package(parent, current_depend->atom);
		Ebuild* resolved = package_resolve_ebuild(pkg, current_depend->atom);
		set_add(ebuild_set, resolved); */
	}
}

void pd_layer_resolve(Emerge* parent, Dependency* depend, Ebuild* target) {
	Vector* ebuild_set = small_map_new(32);
	__pd_layer_resolve__(parent, depend, target, ebuild_set);
}

Package* atom_resolve_package(Emerge* emerge, P_Atom* atom) {
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
	if (!target_pkg)
		portage_die("Package '%s' not found", atom->key);
	
	return target_pkg;
}

int pd_compare_range(int cmp, atom_version_t range) {
	if (cmp == 0 && range & ATOM_VERSION_E)
		return 1;
	else if (cmp > 0 && range & ATOM_VERSION_G)
		return 1;
	else if (cmp < 0 && range & ATOM_VERSION_L)
		return 1;
	return 0;
}

int atom_match_ebuild(Ebuild* ebuild, P_Atom* atom) {
	int slot_cmp = 0;
	int sub_slot_cmp = 0;
	
	if (!atom->slot)
		slot_cmp = 0;
	else {
		slot_cmp = strcmp(ebuild->slot, atom->slot);
		if(atom->sub_slot)
			sub_slot_cmp = strcmp(ebuild->sub_slot, atom->sub_slot);
	}
	
	int cmp = 0;
	int cmp_rev = 0;
	if (atom->version) {
		cmp = atom_version_compare(ebuild->version, atom->version);
		cmp_rev = ebuild->revision - atom->revision;
	} else {
		/* Only compare the slots */
		if (!pd_compare_range(slot_cmp, atom->range))
			return 0;
		if (!pd_compare_range(sub_slot_cmp, atom->range))
			return 0;
		return 1;
	}
	
	if (pd_compare_range(cmp, atom->range))
		return 1;
	
	if (cmp == 0 && atom->range & ATOM_VERSION_E) {
		if (atom->range == ATOM_VERSION_E) {
			if (cmp_rev == 0)
				return 1;
		} else if (atom->range == ATOM_VERSION_REV)
			return 1;
	}
	
	return 0;
}

SelectedEbuild* package_resolve_ebuild(Package* pkg, P_Atom* atom) {
	Ebuild* current;
	for (current = pkg->ebuilds; current; current = current->older) {
		if (atom_match_ebuild(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			if (!current->metadata_init)
				package_metadata_init(current);
			
			for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next)
				if (atom_match_ebuild(current, keyword->atom))
					if (keyword->keywords[pkg->parent->parent->target_arch] < accept_keyword)
						accept_keyword = keyword->keywords[pkg->parent->parent->target_arch];
			if (current->keywords[pkg->parent->parent->target_arch] >= accept_keyword) {
				SelectedEbuild* out = malloc(sizeof(SelectedEbuild));
				out->ebuild = current;
				out->installed = portagedb_resolve_installed(pkg->parent->parent->database, atom);
				
				if (!out->installed)
					return out;
				
				int cmp = ebuild_installedebuild_cmp(out->ebuild, out->installed);
				if (cmp > 0)
					out->action = PORTAGE_UPDATE;
				
				
				return out;
			}
		}
	}
	
	return NULL;
}