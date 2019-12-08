//
// Created by atuser on 12/4/19.
//

#include <stdio.h>
#include <stdlib.h>
#include "dep_v4.h"
#include "package.h"
#include "database.h"
#include "conflict.h"
#include "suggestion.h"
#include "installed_backtrack.h"
#include <string.h>
#include <errno.h>

void resolved_ebuild_init(ResolvedEbuild* parent, ResolvedEbuild* out) {
	/* Build the use flags from the ebuild */
	for (UseFlag* current_use = out->ebuild->use; current_use; current_use = current_use->next) {
		UseFlag* new_flag = use_new(current_use->name, current_use->status, current_use->priority);
		
		new_flag->next = out->useflags;
		out->useflags = new_flag;
	}
	
	struct {
		atom_use_t atom_use;
		use_t resolved_use; /* From parent */
		use_t target_use;
	} atom_use_enum_links[] = {
			{ATOM_USE_DISABLE, -1, USE_DISABLE},
			{ATOM_USE_ENABLE, -1, USE_ENABLE},
			{ATOM_USE_ENABLE_IF_ON, USE_ENABLE, USE_ENABLE},
			{ATOM_USE_DISABLE_IF_OFF, USE_DISABLE, USE_DISABLE},
			{ATOM_USE_EQUAL, USE_DISABLE, USE_DISABLE},
			{ATOM_USE_EQUAL, USE_ENABLE, USE_ENABLE},
			{ATOM_USE_OPPOSITE, USE_DISABLE, USE_ENABLE},
			{ATOM_USE_OPPOSITE, USE_ENABLE, USE_DISABLE},
	};
	
	/* Apply the explicit flags */
	for (AtomFlag* current_use = out->selected_by->atom->useflags; current_use; current_use = current_use->next) {
		/*
		ATOM_USE_DISABLE, //!< atom[-bar]
		ATOM_USE_ENABLE, //!< atom[bar]
		ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
		ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
		ATOM_USE_EQUAL, //!< atom[bar=]
		ATOM_USE_OPPOSITE //!< atom[!bar=]
		
		ATOM_NO_DEFAULT, //!< use
		ATOM_DEFAULT_ON, //!< use(+)
		ATOM_DEFAULT_OFF, //!< use(-)
		
		 */
		UseFlag* parent_flag = NULL;
		if (parent)
			parent_flag = use_get(parent->useflags, current_use->name);
		
		UseFlag* flag = use_get(out->useflags, current_use->name);
		if (!flag && current_use->def != ATOM_NO_DEFAULT) {
			use_t def_status = current_use->def == ATOM_DEFAULT_ON ? USE_ENABLE : USE_DISABLE;
			use_t resolved_status = def_status;
			
			for (int i = 0; i < sizeof(atom_use_enum_links) / sizeof(atom_use_enum_links[0]); i++) {
				if (current_use->option == atom_use_enum_links[i].atom_use
				    && (atom_use_enum_links[i].resolved_use == -1
				        || parent_flag->status == atom_use_enum_links[i].resolved_use))
					resolved_status = atom_use_enum_links[i].target_use;
			}
			
			if (def_status != resolved_status) {
				char* atom_str = atom_get_str(out->selected_by->atom);
				portage_die("Default use for %s did not match %s", current_use->name, atom_str);
				return;
			}
		}
		else if (!flag) /* Invalid flag name, ignore */
			continue;
		else {
			use_t old_status = flag->status;
			
			for (int i = 0; i < sizeof(atom_use_enum_links) / sizeof(atom_use_enum_links[0]); i++) {
				if (current_use->option == atom_use_enum_links[i].atom_use
				    && (atom_use_enum_links[i].resolved_use == -1
				        || parent_flag->status == atom_use_enum_links[i].resolved_use))
					flag->status = atom_use_enum_links[i].target_use;
			}
			
			/* Error if flag was forced */
			if (flag->priority == PRIORITY_FORCE && old_status != flag->status)
				portage_die("Explicit flag %s in %s overriden by profile forced flag", flag->name, out->ebuild->ebuild_key);
		}
		
		if (flag) {
			UseFlag* ex_dup = use_new(flag->name, flag->status, PRIORITY_NORMAL);
			UseReason* reason = use_reason_new(parent, current_use, out->selected_by);
			reason->next = ex_dup->reason;
			ex_dup->reason = reason;
			
			ex_dup->next = out->explicit_flags;
			out->explicit_flags = ex_dup;
		}
	}
	
	if (!ebuild_check_required_use(out)) {
		printf("Seleted by: \n");
		
		for (ResolvedEbuild* selected_by = parent; selected_by; selected_by = selected_by->parent)
			resolved_ebuild_print(selected_by->environ, selected_by);
		
		//if (out->use_change) {
		//	printf("Use flags change by following dependencies");
		//	for (int i = 0; i < out->use_change->n; i++)
		//		resolved_ebuild_resolve(emerge, vector_get(out->, i));
		//}
		
		portage_die("Invalid required use");
	}
	
	if (out->installed && out->action == PORTAGE_REPLACE) {
		for (UseFlag* use = out->useflags; use; use = use->next) {
			UseFlag* ebuild_use_status = use_get(out->installed->use, use->name);
			if (ebuild_use_status && ebuild_use_status->status != use->status) {
				out->action = PORTAGE_USE_FLAG;
				break;
			}
		}
	}
}

int resolve_ebuild_merge(ResolvedEbuild* selected, ResolvedEbuild* merge) {
	UseFlag* prev_conflict;
	UseFlag* curr_conflict;
	conflict_use_check(selected->explicit_flags, merge->explicit_flags, &prev_conflict, &curr_conflict);
	
	/* Atempt to resolve the conflict by backtracking the previous package use flags */
	if (prev_conflict) {
		Suggestion* suggestion = conflict_use_resolve(curr_conflict, prev_conflict->status);
		if (!suggestion)
			suggestion = conflict_use_resolve(prev_conflict, curr_conflict->status);
		
		if (!suggestion) {
			errno = 0;
			plog_error("Selected packages could not resolve use conflict");
			
			char* atom_1 = atom_get_str(selected->selected_by->atom);
			char* atom_2 = atom_get_str(merge->selected_by->atom);
			plog_error("%s DEP = %s wants %c%s", selected->parent->ebuild->ebuild_key, atom_1, prev_conflict->status == USE_ENABLE ? '+' : '-', prev_conflict->name);
			plog_error("%s DEP = %s wants %c%s", merge->parent->ebuild->ebuild_key, atom_2, curr_conflict->status == USE_ENABLE ? '+' : '-', curr_conflict->name);
			
			free(atom_1);
			free(atom_2);
			
			portage_die("Dependency use conflict");
			return 1;
		}
		else {
			suggestion->next = selected->environ->use_suggestions;
			selected->environ->use_suggestions = suggestion;
			
			resolved_ebuild_free(merge);
			return selected->resolve_index;
		}
	}
	
	/* Since there are no errors, just set all the previous package flags out's status */
	for (UseFlag* use = merge->explicit_flags; use; use = use->next) {
		UseFlag* search_prev = use_get(selected->useflags, use->name);
		if (!search_prev) {
			continue;
		}
		search_prev->status = use->status;
	}
	
	if (merge->explicit_flags) {
		//if (!selected->use_change)
		//	selected->use_change = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
		//vector_add(selected->use_change, out);
		
		if (merge->action == PORTAGE_USE_FLAG)
			selected->action = PORTAGE_USE_FLAG;
	}
	else
		resolved_ebuild_free(merge);
	
	return 0;
}

Backtrack* backtrack_new(ResolvedEbuild* ebuild, Package* backtrack_to, backtrack_t action) {
	Backtrack* out = malloc(sizeof(Backtrack));
	
	out->failed = ebuild;
	out->backtrack_to = backtrack_to;
	out->action = action;
	
	return out;
}

ResolvedEbuild* dependency_resolve_installed_backtrack(ResolvedEbuild* resolve) {
	InstalledEbuild* check = resolve->installed;
	if (!check)
		return resolve;
	
	Vector* install_deps = check->required_by;
	
	/* This package is being remove, make sure all
	 * installed deps match the target install
	 * */
	ResolvedEbuild* current = resolve;
	for (; current; current = current->next) {
		int i = 0;
		for (i = 0; i < install_deps->n; i++) {
			InstalledBacktrack* current_backtrack = vector_get(install_deps, i);
			/* Use the selected package if this package is being updated */
			ResolvedEbuild* select = dependency_check_selected(resolve->environ->selected, current);
			
			P_Atom* target_atom = current_backtrack->selected_by->atom;
			if (select)
				target_atom = select->selected_by->atom;
			
			if (!ebuild_match_atom(current->ebuild, target_atom)) {
				plog_warn("Failed %s on %s for %s", target_atom, current->ebuild->ebuild_key, current_backtrack->required_by->parent->key);
				break;
			}
		}
		
		if (i == install_deps->n)
			break;
	}
	
	return current;
}

ResolvedEbuild* dependency_resolve_ebuild(Emerge* emerge, ResolvedEbuild* selected_by, Dependency* dep) {
	ResolvedEbuild* all_ebuilds = resolved_ebuild_resolve(emerge, dep->atom);
	ResolvedEbuild* matching_ebuild = dependency_resolve_installed_backtrack(all_ebuilds);
	
	if (!matching_ebuild) {
		portage_die("No matching ebuild for %s", atom_get_str(dep->atom));
		return NULL;
	}
	
	matching_ebuild->selected_by = dep;
	
	resolved_ebuild_init(selected_by, matching_ebuild);
	
	ResolvedEbuild* prev_sel = dependency_check_selected(emerge->selected, matching_ebuild);
	if (!prev_sel)
		return matching_ebuild;
	
	int merge_res = resolve_ebuild_merge(prev_sel, matching_ebuild);
	if (merge_res != 0) {
		/* portage_die() already called */
		return NULL;
	}
	
	/* There is a previously selected version of this
	 * dont redo the dependencies*/
	return NULL;
}

ResolvedEbuild* dependency_check_selected(Vector* search, ResolvedEbuild* check) {
	/* This must be a tree traversal */
	/* It goes parent pre post (pre order)
	 * This allows a short circuit for package server and log(n) time */
	
	for (int i = 0; i < search->n; i++) {
		ResolvedEbuild* current = vector_get(search, i);
		if (current->ebuild->parent == check->ebuild->parent) {
			/* Short circuit if the same ebuild */
			if (current->ebuild == check->ebuild)
				return current;
			
			if (strcmp(current->ebuild->slot, check->ebuild->slot) == 0)
				return current;
		}
		
		ResolvedEbuild* found = dependency_check_selected(current->pre_dependency, check);
		if (found)
			return found;
		
		found = dependency_check_selected(current->post_depenendcy, check);
		if (found)
			return found;
	}
	
	return NULL;
}

ResolvedEbuild* dependency_check_selected_atom(Vector* search, P_Atom* atom) {
	for (int i = 0; i < search->n; i++) {
		ResolvedEbuild* current = vector_get(search, i);
		if (ebuild_match_atom(current->ebuild, atom))
			return current;
		
		ResolvedEbuild* dep_check = dependency_check_selected_atom(current->pre_dependency, atom);
		if (dep_check)
			return dep_check;
		
		dep_check = dependency_check_selected_atom(current->post_depenendcy, atom);
		if (dep_check)
			return dep_check;
	}
	
	return NULL;
}

void dependency_resolve_block(Emerge* emerge, ResolvedEbuild* blocker, P_Atom* atom) {
	ResolvedEbuild* ebuilds = resolved_ebuild_resolve(emerge, atom);
	for (ResolvedEbuild* e = ebuilds; e; e = e->next) {
		e->ebuild->blocked = 1;
	}
	
	ResolvedEbuild* selected_match = dependency_check_selected_atom(emerge->selected, atom);
	if (!selected_match && atom->blocks & ATOM_BLOCK_SOFT)
		return;
	else if (selected_match) {
		plog_info("%s may not be installed at the same time as %s", selected_match->ebuild->ebuild_key, blocker->ebuild->ebuild_key);
		plog_info("Install %s first then rerun", selected_match->ebuild->ebuild_key);
		plog_info("emerge --oneshot =%s", selected_match->ebuild->ebuild_key);
		portage_die("%s", atom_get_str(atom));
	}
	
	InstalledPackage* pkg = map_get(emerge->database->installed, atom->key);
	if (!pkg)
		return; /* Package not installed */
	
	InstalledEbuild* installed = NULL;
	for (installed = pkg->installed; installed; installed = installed->older_slot) {
		if (portagedb_ebuild_match(installed, atom))
			break;
	}
	
	if (!installed)
		return;
	
	errno = 0;
	plog_error("%s", atom_get_str(atom));
	portage_die("%s may not be installed with %s-%s installed", blocker->ebuild->ebuild_key, installed->parent->key, installed->version->full_version);
}

void dependency_resolve(Emerge* emerge, ResolvedEbuild* parent, Dependency* dependency, Vector* add_to) {
	for (Dependency* current_depend = dependency; current_depend; current_depend = current_depend->next) {
		if (current_depend->depends == HAS_DEPENDS) {
			/* Do logical evaluation for use flags */
			if ((current_depend->selector == USE_DISABLE || current_depend->selector == USE_ENABLE)) {
				UseFlag* u = use_get(parent->useflags, current_depend->target);
				if (u && u->status == current_depend->selector) {
					plog_enter_stack("%c%s?", current_depend->selector == USE_ENABLE ?: '!', current_depend->target);
					dependency_resolve(emerge, parent, current_depend->selectors, add_to);
					plog_exit_stack();
				}
			} else if (current_depend->selector == USE_LEAST_ONE) {
				for (Dependency* least_one_iter = current_depend->selectors; least_one_iter; least_one_iter = least_one_iter->next) {
				
				}
			} else
				portage_die("Illegal operator %s in a dependency expression", current_depend->target);
			continue;
		}
		
		if (current_depend->atom->blocks != ATOM_BLOCK_NONE) {
			dependency_resolve_block(emerge, parent, current_depend->atom);
			continue;
		}
		
		char* atom_stack_str = atom_get_str(current_depend->atom);
		plog_enter_stack("resolve %s", atom_stack_str);
		ResolvedEbuild* se = dependency_resolve_ebuild(emerge, parent, current_depend);
		plog_exit_stack();
		
		if (!se) // This was previously selected
			return;
		
		/*
		plog_enter_stack("backtrack %s", se->ebuild->ebuild_key);
		backtrack_rebuild(parent, se, dependency_order, ebuild_set, blocked_set);
		plog_exit_stack();
		 */
		
		if (se->action != PORTAGE_REPLACE || emerge->options & EMERGE_DEEP) {
			vector_add(add_to, se);
			
			if (!(emerge->options & EMERGE_USE_BINHOST)) {
				plog_enter_stack("bdepend %s", atom_stack_str);
				dependency_resolve(emerge, se, se->ebuild->bdepend, se->pre_dependency);
				plog_exit_stack();
			}
			
			plog_enter_stack("depend %s", atom_stack_str);
			dependency_resolve(emerge, se, se->ebuild->depend, se->pre_dependency);
			plog_exit_stack();
			
			plog_enter_stack("rdepend %s", atom_stack_str);
			dependency_resolve(emerge, se, se->ebuild->rdepend, se->pre_dependency);
			plog_exit_stack();
			
			plog_enter_stack("pdepend %s", atom_stack_str);
			dependency_resolve(emerge, se, se->ebuild->pdepend, se->post_depenendcy);
			plog_exit_stack();
			
			free(atom_stack_str);
		} else {
			if (!parent) { /* Atom pulled in by command line, reinstall it */
				vector_add(add_to, se);
			}
		}
	}
}

void dependency_build_vector(Vector* traverse, Vector* target) {
	/* in-order traversal of dependency tree
	 * pre parent post
	 * */
	
	for (int i = 0; i < traverse->n; i++) {
		ResolvedEbuild* current = vector_get(traverse, i);
		
		dependency_build_vector(current->pre_dependency, target);
		vector_add (target, current);
		dependency_build_vector(current->post_depenendcy, target);
	}
}