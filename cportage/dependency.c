//
// Created by atuser on 5/19/19.
//

#include "dependency.h"
#include "package.h"
#include "portage.h"
#include "database.h"
#include "suggestion.h"
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
		} else if (atom->range == ATOM_VERSION_REV)
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


Ebuild* atom_resolve_ebuild(Emerge* emerge, P_Atom* atom) {
	Package* target_pkg = atom_resolve_package(emerge, atom);
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

PortageDependency* dependency_new(Dependency* parent, Ebuild* e, P_Atom* p, InstalledEbuild* old, dependency_t option) {
	PortageDependency* out = malloc(sizeof(PortageDependency));
	out->selected_by = parent;
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
		dependency_resolve_single(emerge, ebuild, ebuild->rdepend, 0);
		
		/* IM NOT REALLY SURE ABOUT THIS */
		dependency_resolve_single(emerge, ebuild, ebuild->pdepend, 0);
	} else if (emerge->build_opts == EMERGE_BUILDSIDE) {
		dependency_resolve_single(emerge, ebuild, ebuild->bdepend, 0);
		dependency_resolve_single(emerge, ebuild, ebuild->rdepend, 0);
		dependency_resolve_single(emerge, ebuild, ebuild->depend, 0);
		dependency_resolve_single(emerge, ebuild, ebuild->pdepend, 0);
	}
	
	ebuild->resolved = 1;
}

void dependency_useflag_simplify(Ebuild* parent, Ebuild* resolved, P_Atom* atom) {
	/**
	 * ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
	 * ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
	 * ATOM_USE_EQUAL, //!< atom[bar=]
	 * ATOM_USE_OPPOSITE //!< atom[!bar=]
	 */
	
	for (AtomFlag* curr = atom->useflags; curr; curr = curr->next) {
		if (curr->option == ATOM_USE_ENABLE) {
			ebuild_set_use(resolved, curr->name, USE_ENABLE);
			continue;
		} else if (curr->option == ATOM_USE_DISABLE) {
			ebuild_set_use(resolved, curr->name, USE_DISABLE);
			continue;
		}
		
		use_select_t value = ebuild_check_use(parent, curr->name);
		if (value == -1) {
			if (curr->def == ATOM_NO_DEFAULT)
				portage_die("Ebuild %s-%s has no use flag %s and no default is defined", parent->parent->key,
				            parent->version->full_version, curr->name);
			else if (curr->def == ATOM_DEFAULT_ON)
				value = USE_ENABLE;
			else if (curr->def == ATOM_DEFAULT_OFF)
				value = USE_DISABLE;
		}
		
		if (curr->option == ATOM_USE_ENABLE_IF_ON && value == USE_ENABLE)
			ebuild_set_use(resolved, curr->name, USE_ENABLE);
		else if (curr->option == ATOM_USE_DISABLE_IF_OFF && value == USE_DISABLE)
			ebuild_set_use(resolved, curr->name, USE_DISABLE);
		else if (curr->option == ATOM_USE_EQUAL)
			ebuild_set_use(resolved, curr->name, value);
		else if (curr->option == ATOM_USE_OPPOSITE)
			ebuild_set_use(resolved, curr->name, value == USE_ENABLE ? USE_DISABLE : USE_ENABLE);
	}
}

PortageDependency*
dependency_select_atom(Emerge* emerge, Ebuild* parent, Dependency* dep, int try_keyword) {
	Package* package = atom_resolve_package(emerge, dep->atom);
	InstalledEbuild* installed = portagedb_resolve_installed(emerge->database, dep->atom);
	Ebuild* resolved = atom_resolve_ebuild(emerge, dep->atom);
	
	if (!resolved && try_keyword == 1) {
		/* Try again with unstable selected */
		Keyword* suggestion_placeholder = malloc(sizeof(Keyword));
		suggestion_placeholder->next = package->keywords;
		suggestion_placeholder->atom = atom_dup(dep->atom);
		suggestion_placeholder->keywords[emerge->target_arch] = KEYWORD_UNSTABLE;
		
		package->keywords = suggestion_placeholder;
		resolved = atom_resolve_ebuild(emerge, dep->atom);
		
		if (resolved) {
			/* It works with the unstable flag */
			/* Raise the suggestion and continue */
			emerge_suggestion_keyword(emerge, resolved, dep->atom);
		} else
			return NULL;
	} else if (!resolved)
		return NULL;
	
	// Check if this atom is already selected, if so, set it to desired version
	PortageDependency* selected = dependency_set_selected(emerge, resolved, dep->atom);
	if (selected)
		return selected;
	
	PortageDependency* out = NULL;
	
	dependency_t option = -1;
	
	if (!installed)
		option = PORTAGE_NEW;
	else {
		int update = ebuild_installedebuild_cmp(resolved, installed);
		if (update == 0)
			option = PORTAGE_REBUILD;
		else if (update > 0)
			option = PORTAGE_UPDATE;
		else if (update < 0)
			option = PORTAGE_DOWNGRADE;
	}
	
	out = dependency_new(dep, resolved, dep->atom, installed, option);
	
	return out;
}

PortageDependency* dependency_select_one(Emerge* emerge, Ebuild* current_ebuild, Dependency* from) {
	Dependency* current;
	
	PortageDependency* selected = NULL;
	
	/* Search for stable one first */
	for (current = from; current && !selected; current = current->next)
		selected = dependency_resolve_single(emerge, current_ebuild, current, 0);
	
	for (; current; current = current->next) {
		InstalledEbuild* installed = portagedb_resolve_installed(emerge->database, current->atom);
		selected = dependency_get_selected(emerge, current->atom);
		
		if (installed) {
			if (!selected)
				;
			else
				return selected;
		}
	}
}

Vector* dependency_resolve(Emerge* emerge, Ebuild* current_ebuild, Dependency* depends, int try_keyword) {
	Vector* out = vector_new(sizeof(PortageDependency*), VECTOR_ORDERED | VECTOR_REMOVE);
	for (Dependency* current = depends; current; current = current->next) {
		PortageDependency* found = dependency_resolve_single(emerge, )
	}
}

inline PortageDependency* dependency_resolve_single(Emerge* emerge, Ebuild* current_ebuild, Dependency* depend, int try_keyword) {
	if (depend->depends == HAS_DEPENDS) {
		// Check if we are looking for a use flag
		if ((depend->selector == USE_DISABLE || depend->selector == USE_ENABLE)) {
			if (depend->selector == ebuild_check_use(current_ebuild, depend->target)) {
				dependency_resolve(emerge, current_ebuild, depend->selectors, try_keyword);
				
			}
			return NULL;
		}
		else if (depend->selector == USE_LEAST_ONE) {
			return dependency_select_one(emerge, current_ebuild, depend);
		}
		else
			portage_die("Illegal operator %s in a dependency expression", depend->target);
	}
	
	//ATOM
	return dependency_select_atom(emerge, current_ebuild, depend, try_keyword);
}

PortageDependency* dependency_get_selected(Emerge* emerge, P_Atom* search) {
	for (int i = 0; i < emerge->selected->n; i++) {
		PortageDependency* check = *(PortageDependency**) vector_get(emerge->selected, i);
		if (strcmp(search->key, check->target->parent->key) != 0)
			continue;
		
		if (search->slot && strcmp(search->slot, check->target->slot) != 0) { // Different slot
			if (search->sub_slot && check->target->sub_slot && strcmp(search->slot, check->target->slot) != 0)
				continue;
			else if (search->sub_slot || check->target->sub_slot)
				;
			else
				continue;
		}
		
		if (atom_match_ebuild(check->target, search))
			return check;
	}
	
	return NULL;
}

PortageDependency*
dependency_set_selected(Emerge* emerge, Ebuild* potential, P_Atom* potential_atom) {
	for (int i = 0; i < emerge->selected->n; i++) {
		PortageDependency* check = *(PortageDependency**) vector_get(emerge->selected, i);
		if (strcmp(potential->parent->key, check->target->parent->key) != 0)
			continue;
		
		if (strcmp(potential->slot, check->target->slot) != 0) // Different slot
			continue;
		
		if (PORTAGE_INSTALL & check->option) {
			if (atom_match_ebuild(check->target, potential_atom))
				return check;
			
			if (atom_match_ebuild(potential, check->selector)) {
				check->target = potential;
				return check;
			}
		}
		char* temp_str = atom_get_str(potential_atom);
		plog_warn("%s-%s does not match atom %s", check->target->parent->key, check->target->version->full_version, temp_str);
		free(temp_str);
	}
	
	return NULL; //!< Package was not selected
}

void dependency_remove(Emerge* emerge, PortageDependency* depend)