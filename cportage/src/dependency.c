//
// Created by atuser on 12/4/19.
//

#include <stdio.h>
#include <stdlib.h>
#include "dependency.h"
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
	
	if (!resolved_ebuild_use_build(parent, out, out->selected_by->atom))
		portage_die("Failed to build resolved ebuild use");
	
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

void resolve_ebuild_merge(ResolvedEbuild* selected, ResolvedEbuild* merge) {
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
			return;
		}
		else {
			suggestion->next = selected->environ->use_suggestions;
			selected->environ->use_suggestions = suggestion;
			
			resolved_ebuild_free(merge);
			return;
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
		//resolved_ebuild_free(merge);
		;
}

Backtrack* backtrack_new(ResolvedEbuild* ebuild, Package* backtrack_to, backtrack_t action) {
	Backtrack* out = malloc(sizeof(Backtrack));
	
	out->failed = ebuild;
	out->backtrack_to = backtrack_to;
	out->action = action;
	
	return out;
}

void dependency_resolve_installed_backtrack(ResolvedPackage* resolve) {
	Emerge* parent_env = resolve->environ;
	
	/* This package is being remove, make sure all
	 * installed deps match the target install
	 * */
	
	ResolvedEbuild* out = NULL;
	for (ResolvedEbuild* current = resolve->current; current;) {
		Vector* install_deps = current->installed->required_by;
		
		int i = 0;
		for (i = 0; i < install_deps->n; i++) {
			InstalledBacktrack* current_backtrack = vector_get(install_deps, i);
			/* Use the selected package if this package is being updated */
			ResolvedEbuild* select = dependency_check_selected(parent_env->selected, current);
			
			P_Atom* target_atom = current_backtrack->selected_by->atom;
			if (select)
				target_atom = select->selected_by->atom;
			char* atom_str = atom_get_str(target_atom);
			if (!ebuild_match_atom(current->ebuild, target_atom)) {
				plog_warn("Failed %s on %s for %s", atom_str, current->ebuild->ebuild_key, current_backtrack->required_by->parent->key);
				free(atom_str);
				break;
			}
			free(atom_str);
		}
		
		ResolvedEbuild* temp_next = current->next;
		
		if (i != install_deps->n) {
			if (current->back)
				current->back->next = current->next;
			if (current->next) {
				current->next->back = current->back;
				current->next = NULL;
			}
			
			resolved_ebuild_free(current);
		}
		else if (!out)
			out = current;
		
		current = temp_next;
	}
	
	return out;
}

ResolvedPackage* dependency_resolve_ebuild(Emerge* emerge, ResolvedEbuild* selected_by, Dependency* dep) {
	ResolvedPackage* all_ebuilds = resolved_ebuild_resolve(emerge, dep->atom);
	dependency_resolve_installed_backtrack(all_ebuilds);
	
	if (!all_ebuilds->current) {
		portage_die("No matching ebuild for %s", atom_get_str(dep->atom));
		return NULL;
	}
	
	for (ResolvedEbuild* re = all_ebuilds->current; re; re = re->next) {
		resolved_ebuild_print(emerge, re);
	}
	
	//all_ebuilds-> = dep;
	
	resolved_ebuild_init(selected_by, matching_ebuild);
	
	ResolvedEbuild* prev_sel = dependency_check_selected(emerge->selected, matching_ebuild);
	if (!prev_sel)
		return matching_ebuild;
	
	resolve_ebuild_merge(prev_sel, matching_ebuild);
	
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
		
		found = dependency_check_selected(current->post_dependency, check);
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
		
		dep_check = dependency_check_selected_atom(current->post_dependency, atom);
		if (dep_check)
			return dep_check;
	}
	
	return NULL;
}

int dependency_backtrack(Emerge* emerge, ResolvedEbuild* start, int (*condition)(Emerge*, ResolvedEbuild*)) {
	if (!start->next)
		return 0;
	
	ResolvedEbuild* matching_ebuilds = dependency_resolve_installed_backtrack(start->next);
	ResolvedEbuild** set_here = start->vec_ptr;
	ResolvedEbuild* new_resolved = matching_ebuilds;
	
	for (; new_resolved; new_resolved = new_resolved->next) {
		if (!condition(emerge, new_resolved))
			break;
	}
	
	/* No more working packages */
	if (!new_resolved)
		return 0;
	
	new_resolved->vec_ptr = set_here;
	new_resolved->parent = start->parent;
	new_resolved->selected_by = start->selected_by;
	
	resolved_ebuild_init(new_resolved->parent, new_resolved);
	
	if (set_here) {
		*set_here = new_resolved;
		start->next = NULL;
		resolved_ebuild_free(start);
	}
	else {
		portage_die("Programming Error");
	}
	
	char* atom_stack_str = atom_get_str(new_resolved->selected_by->atom);
	int passed = dependency_sub_resolve(emerge, new_resolved, atom_stack_str);
	free(atom_stack_str);
	
	return passed;
}

void prv_unblock_all_ebuilds(Ebuild* middle) {
	Ebuild* start = middle;
	for (; start->newer; start = start->newer);
	
	for (; start; start = start->older)
		start->blocked = 0;
}

int dependency_resolve_block(Emerge* emerge, ResolvedEbuild* blocker, P_Atom* atom) {
	if (atom->range == ATOM_VERSION_ALL && atom->slot)
		atom->range = ATOM_VERSION_E;
	ResolvedEbuild* selected_match = dependency_check_selected_atom(emerge->selected, atom);
	if (!selected_match && atom->blocks == ATOM_BLOCK_SOFT)
		return 1;
	else if (selected_match) {
		for (AtomFlag* af = atom->useflags; af; af = af->next) {
			UseFlag* uf = use_get(selected_match->useflags, af->name);
			if (af->option == ATOM_USE_ENABLE) {
				if (uf->status != USE_ENABLE) {
					return 1;
				}
			}
			else if (af->option == ATOM_USE_DISABLE) {
				if (uf->status != USE_DISABLE)
					return 1;
			}
			else {
				portage_die("Invalid blocker atom flag");
			}
		}
	}
	
	InstalledPackage* pkg = map_get(emerge->database->installed, atom->key);
	
	InstalledEbuild* installed = NULL;
	if (pkg) {
		for (installed = pkg->installed; installed; installed = installed->older_slot) {
			if (portagedb_ebuild_match(installed, atom))
				break;
		}
	}
	
	if (selected_match) {
		ResolvedEbuild* ebuilds = resolved_ebuild_resolve(emerge, atom);
		ebuilds = dependency_resolve_installed_backtrack(ebuilds);
		for (ResolvedEbuild* e = ebuilds; e; e = e->next) {
			AtomFlag* af = NULL;
			for (af = atom->useflags; af; af = af->next) {
				UseFlag* uf = use_get(selected_match->useflags, af->name);
				if (af->option == ATOM_USE_ENABLE) {
					if (uf->status != USE_ENABLE)
						break;
				}
				else if (af->option == ATOM_USE_DISABLE) {
					if (uf->status != USE_DISABLE)
						break;
				}
				else {
					portage_die("Invalid blocker atom flag");
				}
			}
			if (af == NULL) // All passed
				e->ebuild->blocked = 1;
		}
		
		int passed = 0;
		ResolvedEbuild* current_match = selected_match;
		while (!passed && current_match) {
			plog_enter_stack("enter backtrack %s", current_match->ebuild->ebuild_key);
			passed = dependency_backtrack(emerge, current_match, resolved_ebuild_is_blocked);
			plog_exit_stack();
			if (!passed) {
				current_match = current_match->next;
				
				if (current_match) {
					current_match->parent = selected_match->parent;
					current_match->vec_ptr = selected_match->vec_ptr;
				}
			}
		}
		
		if (!blocker->next && !current_match) {
			plog_info("%s may not be installed at the same time as %s", selected_match->ebuild->ebuild_key, blocker->ebuild->ebuild_key);
			plog_info("Install %s first then rerun", selected_match->ebuild->ebuild_key);
			plog_info("emerge --oneshot =%s", selected_match->ebuild->ebuild_key);
			portage_die("%s", atom_get_str(atom));
		}
		
		return 1;
	}
	
	if (!installed)
		return 1;
	
	if (!blocker->next) {
		errno = 0;
		plog_error("%s", atom_get_str(atom));
		portage_die("%s may not be installed with %s-%s installed", blocker->ebuild->ebuild_key, installed->parent->key, installed->version->full_version);
	}
	return 0;
}

int dependency_sub_resolve(Emerge* emerge, ResolvedPackage* parent, char* atom_stack_str) {
	/* Clear old dependencies */
	for (int i = 0; i < parent->pre_dependency->n; i++) {
		ResolvedEbuild* c = vector_get(parent->pre_dependency, i);
		resolved_ebuild_free(c);
	}
	for (int i = 0; i < parent->post_dependency->n; i++) {
		ResolvedEbuild* c = vector_get(parent->post_dependency, i);
		resolved_ebuild_free(c);
	}
	
	parent->pre_dependency->n = 0;
	parent->post_dependency->n = 0;
	
	int stop_resolve = 0;
	
	if (!(emerge->options & EMERGE_USE_BINHOST)) {
		plog_enter_stack("bdepend %s (VER = %s, SLOT=%s)", atom_stack_str, parent->ebuild->version->full_version, parent->ebuild->slot);
		stop_resolve = dependency_resolve(emerge, parent, parent->ebuild->bdepend, parent->pre_dependency);
		plog_exit_stack();
		if (stop_resolve)
			return 0;
	}
	
	plog_enter_stack("depend %s (VER = %s, SLOT=%s)", atom_stack_str, parent->ebuild->version->full_version, parent->ebuild->slot);
	stop_resolve = dependency_resolve(emerge, parent, parent->ebuild->depend, parent->pre_dependency);
	plog_exit_stack();
	if (stop_resolve)
		return 0;
	
	plog_enter_stack("rdepend %s (VER = %s, SLOT=%s)", atom_stack_str, parent->ebuild->version->full_version, parent->ebuild->slot);
	stop_resolve = dependency_resolve(emerge, parent, parent->ebuild->rdepend, parent->pre_dependency);
	plog_exit_stack();
	if (stop_resolve)
		return 0;
	
	plog_enter_stack("pdepend %s (VER = %s, SLOT=%s)", atom_stack_str, parent->ebuild->version->full_version, parent->ebuild->slot);
	stop_resolve = dependency_resolve(emerge, parent, parent->ebuild->pdepend, parent->post_dependency);
	plog_exit_stack();
	if (stop_resolve)
		return 0;
	
	return 1;
}

int dependency_resolve(Emerge* emerge, ResolvedEbuild* parent, Dependency* dependency, Vector* add_to) {
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
				ResolvedEbuild* least_one_c = NULL;
				InstalledEbuild* leasted_one_i = NULL;
				Dependency* least_one_iter = NULL;
				for (least_one_iter = current_depend->selectors; least_one_iter; least_one_iter = least_one_iter->next) {
					//leasted_one_i = portagedb_resolve_installed(emerge->database, least_one_iter->atom, NULL);
					if (leasted_one_i)
						break; /* Satisfied */
				}
				
				if (leasted_one_i)
					;
			} else
				portage_die("Illegal operator %s in a dependency expression", current_depend->target);
			continue;
		}
		
		char* atom_stack_str = atom_get_str(current_depend->atom);
		if (current_depend->atom->blocks != ATOM_BLOCK_NONE) {
			plog_enter_stack("block !%s", atom_stack_str);
			int block_res = dependency_resolve_block(emerge, parent, current_depend->atom);
			plog_exit_stack();
			
			if (!block_res) /* Try a new version */
				return 1;
			continue;
		}
		
		ResolvedEbuild* se = dependency_resolve_ebuild(emerge, parent, current_depend);
		
		if (!se) { // This was previously selected
			free(atom_stack_str);
			continue;
		}
		
		/*
		plog_enter_stack("backtrack %s", se->ebuild->ebuild_key);
		backtrack_rebuild(parent, se, dependency_order, ebuild_set, blocked_set);
		plog_exit_stack();
		 */
		
		if (se->action != PORTAGE_REPLACE || emerge->options & EMERGE_DEEP) {
			se->vec_ptr = (ResolvedEbuild**)vector_add(add_to, se);
			
			int passed = 0;
			for (passed = 0; !passed && ) {
				fflush(stdout);
				passed = dependency_sub_resolve(emerge, se, atom_stack_str);
				if (!passed && !se->next)
					portage_die("Backtracking failed");
				else if (!passed) {
				}
			}
			
			free(atom_stack_str);
		} else {
			if (!parent) /* Atom pulled in by command line, reinstall it */
				se->vec_ptr = (ResolvedEbuild**)vector_add(add_to, se);
		}
	}
	
	return 0;
}

void dependency_build_vector(Vector* traverse, Vector* target) {
	/* in-order traversal of dependency tree
	 * pre parent post
	 * */
	
	for (int i = 0; i < traverse->n; i++) {
		ResolvedEbuild* current = vector_get(traverse, i);
		
		dependency_build_vector(current->pre_dependency, target);
		vector_add (target, current);
		dependency_build_vector(current->post_dependency, target);
	}
}