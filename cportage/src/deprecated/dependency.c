//
// Created by atuser on 12/4/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "dependency.h"
#include "../package.h"
#include "../database.h"
#include "../conflict.h"
#include "../suggestion.h"
#include "../installed_backtrack.h"
#include "selected.h"
#include <string.h>
#include <errno.h>

void resolved_ebuild_init(ResolvedEbuild* parent, ResolvedEbuild* out) {
	/* Build the use flags from the ebuild */
	for (UseFlag* current_use = out->ebuild->use; current_use; current_use = current_use->next) {
		UseFlag* new_flag = use_new(current_use->name, current_use->status, current_use->priority);
		
		new_flag->next = out->useflags;
		out->useflags = new_flag;
	}
	
	out->parent->parents = set_new(selected_set_cmp);
	if (!resolved_ebuild_use_build(out, out->parent->parents)) {
		portage_die("Failed to build resolved ebuild use");
	}
	
	if (!ebuild_check_required_use(out)) {
		printf("Seleted by: \n");
		
		
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

void resolve_package_merge(ResolvedPackage* selected, ResolvedPackage* merge) {
	ResolvedEbuild* working_ebuild = rp_current(selected);
	for (; working_ebuild; working_ebuild = working_ebuild->next) {
		SelectedBy* merge_parent = NULL;
		
		
		for (int i = 0; i < merge->parents->parent->n; i++) {
			merge_parent = set_get(merge->parents, i);
			if (!ebuild_match_atom(working_ebuild->ebuild, merge_parent->selected_by->atom))
				break;
		}
		
		if (!merge_parent)
			break;
	}
	
	if (!working_ebuild) {
		portage_die("Could not merge selected packages (no matching ebuild found)");
	}
	
	int parent_res = resolved_ebuild_use_build(working_ebuild, selected->parents);
	int merge_res = resolved_ebuild_use_build(working_ebuild, merge->parents);
	
	if (!merge_res || !parent_res) {
		portage_die("Failed to merge explicit flags");
	}
	
	rp_current(selected) = working_ebuild;
	set_union(selected->parents, merge->parents);
}

void dependency_resolve_installed_backtrack(ResolvedPackage* resolve) {
	Emerge* parent_env = resolve->environ;
	
	/* This package is being remove, make sure all
	 * installed deps match the target install
	 * */
	
	ResolvedEbuild* out = NULL;
	for (ResolvedEbuild* current = rp_current(resolve); current;) {
		if (!current->installed) {
			if (!out)
				out = current;
			current = current->next;
			continue;
		}
		Vector* install_deps = current->installed->required_by;
		
		int i = 0;
		for (i = 0; i < install_deps->n; i++) {
			InstalledBacktrack* current_backtrack = vector_get(install_deps, i);
			
			char* selected_atom_raw = NULL;
			asprintf(&selected_atom_raw, "%s:%s", current_backtrack->required_by->parent->key, current_backtrack->required_by->slot);
			P_Atom* selected_atom = atom_new(selected_atom_raw);
			
			/* Use the selected package if this package is being updated */
			ResolvedPackage* select = selected_get_atom(parent_env->selected, selected_atom);
			
			atom_free(selected_atom);
			free(selected_atom_raw);
			
			if (select)
				continue;
			
			P_Atom* target_atom = current_backtrack->selected_by->atom;
			char* atom_str = atom_get_str(target_atom);
			if (!ebuild_match_atom(current->ebuild, target_atom)) {
				plog_debug("Failed %s on %s for %s", atom_str, current->ebuild->ebuild_key, current_backtrack->required_by->parent->key);
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
				current->back = NULL;
			}
			
			resolved_ebuild_free(current);
		}
		else if (!out)
			out = current;
		
		current = temp_next;
	}
	
	//resolve->ebuilds = out;
	//rp_current(resolve) = out;
}

int dependency_resolve_ebuild(Emerge* emerge, ResolvedPackage* selected_by, Dependency* dep, ResolvedPackage** res,
                              Vector* add_to) {
	*res = resolved_ebuild_resolve(emerge, dep->atom);
	if (!(*res)) {
		plog_warn("No ebuilds for %s", atom_get_str(dep->atom));
		return 0;
	}
	
	//dependency_resolve_installed_backtrack(*res);
	
	if (!rp_current(*res)) {
		plog_warn("Slot conflict %s", atom_get_str(dep->atom));
		return 0;
	}
	
	selected_register_parent(*res, selected_by_new(selected_by, dep, add_to));
	resolved_ebuild_init(selected_by ? rp_current(selected_by) : NULL, rp_current(*res));
	ResolvedPackage* prev_sel = selected_get(emerge->selected, (*res));
	if (!prev_sel)
		return 1;
	
	resolve_package_merge(prev_sel, (*res));
	
	if (!rp_current(prev_sel))
		portage_die("Failed to merge");
	
	/* There is a previously selected version of this
	 * dont redo the dependencies*/
	return 2;
}

int dependency_backtrack(Emerge* emerge, ResolvedPackage* parent, ResolvedPackage* start) {
	if (rp_current(start)->ebuild->inside_block)
		return 0;
	
	dependency_resolve_installed_backtrack(start);
	
	/* No more working packages */
	if (!rp_current(start))
		return 0;
	
	rp_current(start)->ebuild->inside_block = 1;
	resolved_ebuild_init(rp_current(parent), rp_current(start));
	
	char* atom_stack_str = rp_current(start)->ebuild->ebuild_key;
	int passed = dependency_sub_resolve(emerge, start, atom_stack_str);
	rp_current(start)->ebuild->inside_block = 0;
	
	return passed;
}

int dependency_resolve_block(Emerge* emerge, ResolvedPackage* blocker, P_Atom* atom) {
	ResolvedPackage* selected_match = selected_get_atom(emerge->selected, atom);
	if (!selected_match && atom->blocks == ATOM_BLOCK_SOFT)
		return 1;
	else if (selected_match) {
		for (AtomFlag* af = atom->useflags; af; af = af->next) {
			UseFlag* uf = use_get(rp_current(selected_match)->useflags, af->name);
			if (!uf)
				continue;
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
		int passed = 0;
		ResolvedEbuild* old_selected = rp_current(selected_match);
		if (!old_selected) {
			rp_current(selected_match) = selected_match->ebuilds;
			old_selected = selected_match->ebuilds;
		}
		
		for (; rp_current(selected_match); rp_next(selected_match)) {
			plog_enter_stack("enter backtrack %s", rp_current(selected_match)->ebuild->ebuild_key);
			passed = dependency_backtrack(emerge, blocker, selected_match);
			plog_exit_stack();
			
			if (passed)
				break;
			
		}
		
		if (!rp_current(blocker)->next && !selected_match)) {
			plog_info("%s may not be installed at the same time as %s", old_selected->ebuild->ebuild_key, blocker)->ebuild->ebuild_key);
			plog_info("Install %s first then rerun", old_selected->ebuild->ebuild_key);
			plog_info("emerge --oneshot =%s", old_selected->ebuild->ebuild_key);
			portage_die("%s", atom_get_str(atom));
		}
		
		return 1;
	}
	
	if (!installed)
		return 1;
	
	if (!rp_current(blocker)->next) {
		errno = 0;
		plog_error("%s", atom_get_str(atom));
		portage_die("%s may not be installed with %s-%s installed", rp_current(blocker)->ebuild->ebuild_key, installed->parent->key, installed->version->full_version);
	}
	return 0;
}

int dependency_sub_resolve(Emerge* emerge, ResolvedPackage* parent, char* atom_stack_str) {
	/* Clear old dependencies */
	resolved_package_reset_children(parent);
	
	int passed = 0;
	
	if (!(emerge->options & EMERGE_USE_BINHOST)) {
		plog_enter_stack("bdepend %s (VER = %s, SLOT=%s)", atom_stack_str, rp_current(parent)->ebuild->version->full_version, rp_current(parent)->ebuild->slot);
		passed = dependency_resolve(emerge, parent, rp_current(parent)->ebuild->bdepend, parent->bdepend);
		plog_exit_stack();
		if (!passed)
			goto error;
	}
	
	plog_enter_stack("depend %s (VER = %s, SLOT=%s)", atom_stack_str, rp_current(parent)->ebuild->version->full_version, rp_current(parent)->ebuild->slot);
	passed = dependency_resolve(emerge, parent, rp_current(parent)->ebuild->depend, parent->depend);
	plog_exit_stack();
	if (!passed)
		goto error;
	
	plog_enter_stack("rdepend %s (VER = %s, SLOT=%s)", atom_stack_str, rp_current(parent)->ebuild->version->full_version, rp_current(parent)->ebuild->slot);
	passed = dependency_resolve(emerge, parent, rp_current(parent)->ebuild->rdepend, parent->rdepend);
	plog_exit_stack();
	if (!passed)
		goto error;
	
	plog_enter_stack("pdepend %s (VER = %s, SLOT=%s)", atom_stack_str, rp_current(parent)->ebuild->version->full_version,rp_current(parent)->ebuild->slot);
	passed = dependency_resolve(emerge, parent, rp_current(parent)->ebuild->pdepend, parent->pdepend);
	plog_exit_stack();
	if (!passed)
		goto error;
	
	return 1;
	
error:
	resolved_package_reset_children(parent);
	
	return 0;
}

int check_satified(Emerge* emerge, Dependency* dep) {
	if (dep->depends == HAS_DEPENDS) {
		for (Dependency* curr = dep->selectors; curr; curr = curr->next)
			if (!check_satified(emerge, curr))
				return 0;
	}
	else {
		InstalledEbuild* e = portagedb_resolve_installed(emerge->database, dep->atom, NULL);
		if (e)
			return 1;
		
		ResolvedPackage* p = selected_get_atom(emerge->selected, dep->atom);
		if (p)
			return 1;
		
		return 0;
	}
	
	return 1;
}

int dependency_resolve(Emerge* emerge, ResolvedPackage* parent, Dependency* dependency, Vector* add_to) {
	for (Dependency* current_depend = dependency; current_depend; current_depend = current_depend->next) {
		if (current_depend->depends == HAS_DEPENDS) {
			/* Do logical evaluation for use flags */
			if ((current_depend->selector == USE_DISABLE || current_depend->selector == USE_ENABLE)) {
				UseFlag* u = use_get(rp_current(parent)->useflags, current_depend->target);
				if (u && u->status == current_depend->selector) {
					//plog_enter_stack("%c%s?", current_depend->selector == USE_ENABLE ?: '!', current_depend->target);
					if (!dependency_resolve(emerge, parent, current_depend->selectors, add_to))
						portage_die("Failed to resolve");
					//plog_exit_stack();
				}
			} else if (current_depend->selector == USE_LEAST_ONE) {
				int satisfied = 0;
				Dependency* satisfied_dep = current_depend->selectors;
				for (; satisfied_dep && !satisfied; satisfied_dep = satisfied_dep->next)
					satisfied = check_satified(emerge, satisfied_dep);
				
				if (satisfied)
					continue;
				
				/* Not satisfied, try all of them */
				satisfied_dep = current_depend->selectors;
				int passed = 0;
				while (!passed && satisfied_dep) {
					passed = dependency_resolve(emerge, parent, satisfied_dep, add_to);
					if (!passed) {
						plog_warn("Failed to satisfy dep %s", dependency_get_str(satisfied_dep));
						satisfied_dep = satisfied_dep->next;
					}
				}
				
				if (!passed) {
					portage_die("Failed to resolve multi dep for ebuild %s", parent)->ebuild->ebuild_key);
				}
				
			} else if (current_depend->target == NULL) {
				if (dependency_resolve(emerge, parent, current_depend->selectors, add_to) == 0)
					goto error;
			}
			else
				portage_die("Illegal operator %s in a dependency expression", current_depend->target);
			continue;
		}
		
		char* atom_stack_str = atom_get_str(current_depend->atom);
		if (current_depend->atom->blocks != ATOM_BLOCK_NONE) {
			plog_enter_stack("block !%s", atom_stack_str);
			int block_res = dependency_resolve_block(emerge, parent, current_depend->atom);
			plog_exit_stack();
			
			if (!block_res)  {/* Try a new version */
				plog_warn("Block error");
				goto error;
			}
			continue;
		}
		
		ResolvedPackage* se = NULL;
		plog_enter_stack(atom_stack_str);
		int res = dependency_resolve_ebuild(emerge, parent, current_depend, &se, NULL);
		if (res == 0) {
			plog_exit_stack();
			goto error;
		}
		
		else if (res == 2) { // This was previously selected
			plog_exit_stack();
			free(atom_stack_str);
			continue;
		}
		
		
		
		if (rp_current(se)->action != PORTAGE_REPLACE || emerge->options & EMERGE_DEEP) {
			se->remove_index = add_to->n;
			se->added_to = add_to;
			vector_add(add_to, se);
			
			selected_select(emerge->selected, se);
			
			int passed = 0;
			for (; rp_current(se); rp_current(se) = rp_current(se)->next) {
				passed = dependency_sub_resolve(emerge, se, atom_stack_str);
				if (passed)
					break;
			}
			
			if (!passed)
				portage_die("Backtracking failed");
			plog_exit_stack();
			
			free(atom_stack_str);
		} else {
			if (!parent)  {/* Atom pulled in by command line, reinstall it */
				se->remove_index = add_to->n;
				se->added_to = add_to;
				vector_add(add_to, se);
				
				selected_select(emerge->selected, se);
			}
		}
	}
	
	return 1;

error:
	for (int i = add_to->n - 1; i >= 0; i--) {
		selected_unregister_parent(vector_get(add_to, i), parent);
	}
	plog_info("Erororororor");
	fflush(stdout);
	
	return 0;
}

void dependency_build_vector(Vector* traverse, Vector* target) {
	/* in-order traversal of dependency tree
	 * pre parent post
	 * */
	
	for (int i = 0; i < traverse->n; i++) {
		ResolvedPackage* current = vector_get(traverse, i);
		if (!rp_current(current)) {
			char* atom_str = atom_get_str(((SelectedBy*)set_get(current->parents, 0))->selected_by->atom);
			portage_die("No matching ebuild for %s", atom_str);
			continue;
		}
		dependency_build_vector(current->bdepend, target);
		dependency_build_vector(current->depend, target);
		dependency_build_vector(current->rdepend, target);
		vector_add (target, current));
		dependency_build_vector(current->pdepend, target);
	}
}