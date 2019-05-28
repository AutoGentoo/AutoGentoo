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
		dependency_resolve(emerge, ebuild, ebuild->rdepend);
		
		/* IM NOT REALLY SURE ABOUT THIS */
		dependency_resolve(emerge, ebuild, ebuild->pdepend);
	} else if (emerge->build_opts == EMERGE_BUILDSIDE) {
		dependency_resolve(emerge, ebuild, ebuild->bdepend);
		dependency_resolve(emerge, ebuild, ebuild->rdepend);
		dependency_resolve(emerge, ebuild, ebuild->depend);
		dependency_resolve(emerge, ebuild, ebuild->pdepend);
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
		}
		else if (curr->option == ATOM_USE_DISABLE) {
			ebuild_set_use(resolved, curr->name, USE_DISABLE);
			continue;
		}
		
		use_select_t value = ebuild_check_use(parent, curr->name);
		if (value == -1) {
			if (curr->def == ATOM_NO_DEFAULT)
				portage_die("Ebuild %s-%s has no use flag %s and no default is defined", parent->parent->key, parent->version->full_version, curr->name);
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
dependency_select_atom(Emerge* emerge, Ebuild* parent, Ebuild* resolved, InstalledEbuild* installed, Dependency* dep,
                       PortageDependency* selected, int try_keyword) {
	Package* package = atom_resolve_package(emerge, dep->atom);
	
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
		}
		else
			return NULL;
	}
	else if (!resolved)
		return NULL;
	
	if (!selected)
		selected = dependency_check_selected(emerge, resolved, NULL);
	
	PortageDependency* out = NULL;
	
	if (!installed) {
		out = dependency_new(NULL, resolved, dep->atom, NULL, PORTAGE_NEW);
		vector_add(emerge->selected, &dep);
		return out;
	}
	
	dependency_t option = -1;
	int update = ebuild_installedebuild_cmp(resolved, installed);
	if (update == 0)
		option = PORTAGE_REBUILD;
	else if (update > 0)
		option = PORTAGE_UPDATE;
	else if (update < 0)
		option = PORTAGE_DOWNGRADE;
	
	return out;
}

PortageDependency* dependency_select_one(Emerge* emerge, Ebuild* current_ebuild, Dependency* from) {
	P_Atom* resolved_atom = NULL;
	Ebuild* resolved = NULL;
	InstalledEbuild* installed = NULL;
	
	/* Won't be null if already selected */
	PortageDependency* selected = NULL;
	
	Dependency* check = NULL;
	
	/* Search the array for the first one that hits */
	for (check = from; check; check = check->next) {
		if (check->depends != IS_ATOM)
			portage_die("Invalid any of many expression in package %s-%s", current_ebuild->parent->key,
			            current_ebuild->version);
		
		InstalledEbuild* check_installed = portagedb_resolve_installed(emerge->database, check->atom);
		Ebuild* check_resolved = atom_resolve_ebuild(emerge, check->atom);
		
		if (!check_resolved)
			continue;
		
		resolved = check_resolved;
		resolved_atom = check->atom;
		
		if (check_installed)
			installed = check_installed;
		
		selected = dependency_check_selected(emerge, check_resolved, NULL);
		
		
		if (installed || selected)
			break;
	}
	
	/* Go through the rest of the array to make sure no other package is selected or installed */
	for (; check; check = check->next) {
		Ebuild* check_resolved = atom_resolve_ebuild(emerge, check->atom);
		InstalledEbuild* check_installed = portagedb_resolve_installed(emerge->database, check->atom);
		
		dependency_t options = 0;
		PortageDependency* check_seleted = dependency_check_selected(emerge, check_resolved, &options);
		
		if (check_seleted || check_installed) {
			plog_warn("|| (%s-%s %s-%s) in ebuild %s-%s selected multiple packages", check->atom->key, check->atom->version->full_version, resolved->parent->key, resolved->version->full_version);
			portage_die("%s-%s cannot be installed on the same system as %s-%s", check->atom->key, check->atom->version->full_version, resolved->parent->key, resolved->version->full_version);
		}
	}
	
	if (installed) {
		if (!dependency_select_atom(emerge, current_ebuild, resolved, installed, from, NULL, 1))
			portage_die("Failed select a package update for %s-%s", resolved->parent->key, resolved->version->full_version);
	}
	else {
		PortageDependency* new_dep = NULL;
		for (Dependency* possible_install = current->selectors; possible_install; possible_install = possible_install->next) {
			Ebuild* check_resolved = atom_resolve_ebuild(emerge, possible_install->atom);
			if ((new_dep = dependency_select_atom(emerge, current_ebuild, check_resolved, NULL, possible_install, NULL,
			                                      0)))
				break; // Select the first one that works
		}
		
		if (!new_dep) {
			/* Try again by with unstable keywords this time */
			for (Dependency* possible_install = current->selectors; possible_install; possible_install = possible_install->next) {
				if ((new_dep = dependency_select_atom(emerge, current_ebuild, NULL, NULL, possible_install, NULL, 1)))
					break; // Select the first one that works
			}
		}
		
		if (!new_dep)
			portage_die ("Could not find unmasked package in || statement (%s-%s)", current_ebuild->parent->key, current_ebuild->version->full_version);
	}
}

void dependency_resolve(Emerge* emerge, Ebuild* current_ebuild, Dependency* depends) {
	Dependency* current;
	for (current = depends; current; current = current->next) {
		if (current->depends == HAS_DEPENDS) {
			if ((current->selector == USE_DISABLE || current->selector == USE_ENABLE)
			    && current->selector == ebuild_check_use(current_ebuild, current->target)) {
				dependency_resolve(emerge, current_ebuild, current->selectors);
			} else if (current->selector == USE_LEAST_ONE) {
			
			} else
				portage_die("Illegal operator %s in a dependency expression", current->target);
		} else {
			// ATOM
			InstalledEbuild* installed = portagedb_resolve_installed(emerge->database, current->atom);
			Ebuild* resolved = atom_resolve_ebuild(emerge, current->atom);
			
			if (!dependency_select_atom(emerge, current_ebuild, resolved, installed, current, NULL, 1))
				portage_die("All packages to resolve %s have been masked", atom_get_str(current->atom));
		}
	}
}

PortageDependency* dependency_check_selected(Emerge* emerge, Ebuild* potential, dependency_t* options) {
	for (int i = 0; i < emerge->selected->n; i++) {
		PortageDependency* check = *(PortageDependency**) vector_get(emerge->selected, i);
		if (strcmp(potential->parent->key, check->target->parent->key) != 0)
			continue;
		
		if (strcmp(potential->slot, check->target->slot) != 0) {// Different slot
			if (options)
				*options = PORTAGE_SLOT;
			return NULL;
		}
		
		if (PORTAGE_INSTALL & check->option) {
			int ver_cmp = atom_version_compare(check->target->version, potential->version);
			if (options) {
				if (ver_cmp == 0)
					*options = PORTAGE_REPLACE; //!< Already selected
				else if (ver_cmp < 0)
					*options = PORTAGE_UPDATE;
				else if (ver_cmp > 0)
					*options = PORTAGE_DOWNGRADE;
			}
			return check;
		}
	}
	
	if (options)
		*options = 0;
	return NULL; //!< Package was not selected
}