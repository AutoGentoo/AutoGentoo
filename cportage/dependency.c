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

SelectedEbuild* pd_resolve_single(Emerge* emerge, SelectedEbuild* parent_ebuild, Dependency* dep, Vector* selected) {
	Package* pkg = atom_resolve_package(emerge, dep->atom);
	if (!pkg)
		portage_die("Package '%s' not found", dep->atom->key);
	
	SelectedEbuild* out = package_resolve_ebuild(pkg, dep->atom);
	if (!out) {
		errno = 0;
		char* atom_str = atom_get_str(dep->atom);
		plog_error("All ebuilds matching %s have been masked", atom_str);
		if (parent_ebuild)
			plog_error("Required by %s", parent_ebuild->ebuild->ebuild_key);
		
		free(atom_str);
		portage_die("Masked package");
	}
	
	package_metadata_init(out->ebuild);
	
	out->selected_by = dep;
	out->useflags = NULL;
	out->parent_ebuild = parent_ebuild;
	
	/* Build the use flags from the ebuild */
	for (UseFlag* current_use = out->ebuild->use; current_use; current_use = current_use->next) {
		UseFlag* new_flag = malloc(sizeof(UseFlag));
		
		new_flag->next = out->useflags;
		new_flag->name = strdup(current_use->name);
		new_flag->status = current_use->status;
		
		out->useflags = new_flag;
	}
	
	/* Apply the explicit flags */
	for (AtomFlag* current_use = dep->atom->useflags; current_use; current_use = current_use->next) {
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
		if (parent_ebuild)
			parent_flag = get_use(parent_ebuild->useflags, current_use->name);
		UseFlag* flag = get_use(out->useflags, current_use->name);
		if (!flag && current_use->def != ATOM_NO_DEFAULT) {
			use_select_t def_status = current_use->def == ATOM_DEFAULT_ON ? USE_ENABLE : USE_DISABLE;
			use_select_t resolved_status = def_status;
			
			if (current_use->option == ATOM_USE_ENABLE)
				resolved_status = USE_ENABLE;
			else if (current_use->option == ATOM_USE_DISABLE)
				resolved_status = USE_DISABLE;
			else if (current_use->option == ATOM_USE_ENABLE_IF_ON) {
				if (parent_flag && parent_flag->status == USE_ENABLE)
					resolved_status = USE_ENABLE;
			} else if (current_use->option == ATOM_USE_DISABLE_IF_OFF) {
				if (parent_flag && parent_flag->status == USE_DISABLE)
					resolved_status = USE_DISABLE;
			} else if (current_use->option == ATOM_USE_EQUAL)
				resolved_status = parent_flag->status;
			else if (current_use->option == ATOM_USE_OPPOSITE)
				resolved_status = parent_flag->status == USE_ENABLE ? USE_DISABLE : USE_ENABLE;
			
			if (def_status != resolved_status) {
				char* atom_str = atom_get_str(dep->atom);
				portage_die("Default use for %s did not match %s", current_use->name, atom_str);
			}
		}
		else if (!flag)
			continue;
		else {
			use_select_t old_status = flag->status;
			if (current_use->option == ATOM_USE_ENABLE)
				flag->status = USE_ENABLE;
			else if (current_use->option == ATOM_USE_DISABLE)
				flag->status = USE_DISABLE;
			else if (current_use->option == ATOM_USE_ENABLE_IF_ON) {
				if (parent_flag && parent_flag->status == USE_ENABLE)
					flag->status = USE_ENABLE;
			} else if (current_use->option == ATOM_USE_DISABLE_IF_OFF) {
				if (parent_flag && parent_flag->status == USE_DISABLE)
					flag->status = USE_DISABLE;
			} else if (current_use->option == ATOM_USE_EQUAL)
				flag->status = parent_flag->status;
			else if (current_use->option == ATOM_USE_OPPOSITE)
				flag->status = parent_flag->status == USE_ENABLE ? USE_DISABLE : USE_ENABLE;
			
			/* Error if flag was forced */
			if (flag->priority == PRIORITY_FORCE && old_status != flag->status)
				portage_die("Explicit flag %s in %s overriden by profile forced flag", flag->name, out->ebuild->ebuild_key);
		}
		
		if (flag) {
			UseFlag* ex_dup = useflag_new(flag->name, flag->status, PRIORITY_NORMAL);
			ex_dup->next = out->explicit_flags;
			out->explicit_flags = ex_dup;
		}
		
	}
	
	if (!ebuild_check_required_use(out)) {
		printf("Seleted by: \n");
		
		for (SelectedEbuild* selected_by = parent_ebuild; selected_by; selected_by = selected_by->parent_ebuild)
			selected_ebuild_print(emerge, selected_by);
		
		if (out->use_change) {
			printf("Use flags change by following dependencies");
			for (int i = 0; i < out->use_change->n; i++)
				selected_ebuild_print(emerge, vector_get(out->use_change, i));
		}
		
		portage_die("Invalid required use");
	}
	
	if (out->installed && out->action == PORTAGE_REPLACE) {
		for (UseFlag* use = out->useflags; use; use = use->next) {
			UseFlag* ebuild_use_status = get_use(out->installed->use, use->name);
			if (ebuild_use_status && ebuild_use_status->status != use->status) {
				out->action = PORTAGE_USE_FLAG;
				break;
			}
		}
	}
	
	/* Check if slot is already selected */
	SelectedEbuild* prev_sel = pd_check_selected(selected, out);
	
	/* Not selected yet, good to go! */
	if (!prev_sel) {
		return out;
	}
	
	UseFlag* sel_explicit_key = NULL;
	UseFlag* out_explicit_key = NULL;
	for (sel_explicit_key = prev_sel->explicit_flags; sel_explicit_key; sel_explicit_key = sel_explicit_key->next) {
		for (out_explicit_key = out->explicit_flags; out_explicit_key; out_explicit_key = out_explicit_key->next) {
			if (strcmp(sel_explicit_key->name, out_explicit_key->name) == 0 &&
			    sel_explicit_key->status != out_explicit_key->status)
				break;
		}
		
		if (out_explicit_key) /* Error */
			break;
	}
	
	if (sel_explicit_key) /* Error */ {
		errno = 0;
		plog_error("Selected packages could not resolve use conflict");
		
		char* atom_1 = atom_get_str(prev_sel->selected_by->atom);
		char* atom_2 = atom_get_str(out->selected_by->atom);
		plog_error("%s DEP = %s wants %c%s", prev_sel->parent_ebuild->ebuild->ebuild_key, atom_1, sel_explicit_key->status == USE_ENABLE ? '+' : '-', sel_explicit_key->name);
		plog_error("%s DEP = %s wants %c%s", out->parent_ebuild->ebuild->ebuild_key, atom_2, out_explicit_key->status == USE_ENABLE ? '+' : '-', out_explicit_key->name);
		
		free(atom_1);
		free(atom_2);
		
		portage_die("Dependency use conflict");
	}
	
	for (UseFlag* use = out->explicit_flags; use; use = use->next) {
		UseFlag* search_prev = get_use(prev_sel->useflags, use->name);
		if (!search_prev) {
			continue;
		}
		search_prev->status = use->status;
	}
	
	if (out->explicit_flags) {
		if (!prev_sel->use_change)
			prev_sel->use_change = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
		vector_add(prev_sel->use_change, out);
		
		if (out->action == PORTAGE_USE_FLAG)
			prev_sel->action = PORTAGE_USE_FLAG;
	}
	else {
		selected_ebuild_free(out);
	}
	
	return NULL;
}

void __pd_layer_resolve__(Emerge* parent, Dependency* depend, SelectedEbuild* target, Vector* ebuild_set,
                          Vector* blocked_set, Vector* dependency_order) {
	for (Dependency* current_depend = depend; current_depend; current_depend = current_depend->next) {
		if (current_depend->depends == HAS_DEPENDS) {
			/* Do logical evaluation for use flags */
			if ((current_depend->selector == USE_DISABLE || current_depend->selector == USE_ENABLE)) {
				UseFlag* u = get_use(target->useflags, current_depend->target);
				if (u && u->status == current_depend->selector) {
					plog_enter_stack("%c%s ?", current_depend->selector == USE_ENABLE ? ' ' : '!', current_depend->target);
					__pd_layer_resolve__(parent, current_depend->selectors, target, ebuild_set, blocked_set, dependency_order);
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
			Package* resolved_package = atom_resolve_package(parent, current_depend->atom);
			if (!resolved_package)
				continue;
			
			for (int i = 0; i < ebuild_set->n; i++) {
				SelectedEbuild* current_se = vector_get(ebuild_set, i);
				if (resolved_package != current_se->ebuild->parent)
					continue;
				
				if (atom_match_ebuild(current_se->ebuild, current_depend->atom)) {
					int use_match = 1;
					for (AtomFlag* af = current_depend->atom->useflags; af; af = af->next) {
						use_select_t use_value = USE_DISABLE;
						if (af->def == ATOM_DEFAULT_ON)
							use_value = USE_ENABLE;
						else if (af->def == ATOM_DEFAULT_OFF)
							use_value = USE_DISABLE;
						
						UseFlag* use = get_use(current_se->useflags, af->name);
						if (use)
							use_value = use->status;
						
						if (af->option == ATOM_USE_ENABLE && use_value != USE_ENABLE) {
							use_match = 0;
							break;
						} else if (af->option == ATOM_USE_DISABLE && use_value != USE_DISABLE) {
							use_match = 0;
							break;
						} else {
							plog_warn("Blocker uses something ebuild conditional");
							plog_warn("If this ever comes up ima be pissed");
						}
					}
					
					int slot_match = 1;
					if (current_depend->atom->slot && strcmp(current_depend->atom->slot, current_se->ebuild->slot) != 0)
						slot_match = 0;
					else if (current_depend->atom->sub_slot &&
					         strcmp(current_depend->atom->sub_slot, current_se->ebuild->sub_slot) != 0)
						slot_match = 0;
					
					if (!use_match || !slot_match)
						continue;
					
					char* blocker = atom_get_str(target->selected_by->atom);
					char* blocked_pkg = current_se->ebuild->ebuild_key;
					if (current_depend->atom->blocks == ATOM_BLOCK_SOFT) {
						plog_info("%s may not be installed at the same time as %s", blocked_pkg, blocker);
						plog_info("Install %s first then rerun", blocked_pkg);
						plog_info("emerge --oneshot %s", blocked_pkg);
						
					} else if (current_depend->atom->blocks == ATOM_BLOCK_HARD)
						portage_die("%s may not be installed with %s installed", blocked_pkg, blocker);
					
					free(blocker);
					
					current_se->action = PORTAGE_BLOCK;
					vector_add(blocked_set, current_se);
				}
			}
			
			
			/* Check for installed package for hard blockers */
			continue;
		}
		
		char* atom_stack_str = atom_get_str(current_depend->atom);
		plog_enter_stack("resolve %s", atom_stack_str);
		SelectedEbuild* se = pd_resolve_single(parent, target, current_depend, ebuild_set);
		plog_exit_stack();
		
		/* Could be dangerous because use flags could change dependencies */
		if (!se) /* Already resolved */
			continue;
		
		vector_add(ebuild_set, se);
		
		
		plog_enter_stack("bdepend %s", atom_stack_str);
		__pd_layer_resolve__(parent, se->ebuild->bdepend, se, ebuild_set, blocked_set, dependency_order);
		plog_exit_stack();
		
		plog_enter_stack("depend %s", atom_stack_str);
		__pd_layer_resolve__(parent, se->ebuild->depend, se, ebuild_set, blocked_set, dependency_order);
		plog_exit_stack();
		
		plog_enter_stack("rdepend %s", atom_stack_str);
		__pd_layer_resolve__(parent, se->ebuild->rdepend, se, ebuild_set, blocked_set, dependency_order);
		plog_exit_stack();
		
		plog_enter_stack("pdepend %s", atom_stack_str);
		__pd_layer_resolve__(parent, se->ebuild->pdepend, se, ebuild_set, blocked_set, dependency_order);
		plog_exit_stack();
		
		free(atom_stack_str);
		
		vector_add(dependency_order, se);
	}
}

Vector* pd_layer_resolve(Emerge* parent, Dependency* depend) {
	Vector* ebuild_set = vector_new(VECTOR_ORDERED | VECTOR_KEEP);
	Vector* blocked_set = vector_new(VECTOR_ORDERED | VECTOR_KEEP);
	Vector* dep_order = vector_new(VECTOR_ORDERED | VECTOR_KEEP);
	
	__pd_layer_resolve__(parent, depend, NULL, ebuild_set, blocked_set, dep_order);
	
	for (int i = 0; i < blocked_set->n; i++) {
		vector_add(dep_order, vector_get(blocked_set, i));
	}
	
	vector_free(blocked_set);
	vector_free(ebuild_set);
	
	return dep_order;
}

Package* atom_resolve_package(Emerge* emerge, P_Atom* atom) {
	Repository* repo = NULL;
	for (Repository* current = emerge->repos; current; current = current->next) {
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

SelectedEbuild* package_resolve_ebuild(Package* pkg, P_Atom* atom) {
	Ebuild* current;
	for (current = pkg->ebuilds; current; current = current->older) {
		package_metadata_init(current);
		if (atom_match_ebuild(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			if (!current->metadata_init)
				package_metadata_init(current);
			
			/* Apply package.accept_keywords */
			for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next) {
				if (atom_match_ebuild(current, keyword->atom))
					if (keyword->keywords[pkg->parent->parent->target_arch] < accept_keyword)
						accept_keyword = keyword->keywords[pkg->parent->parent->target_arch];
			}
			
			if (current->keywords[pkg->parent->parent->target_arch] >= accept_keyword) {
				SelectedEbuild* out = malloc(sizeof(SelectedEbuild));
				out->useflags = NULL;
				out->explicit_flags = NULL;
				out->parent_ebuild = NULL;
				out->selected_by = NULL;
				out->use_change = NULL;
				out->ebuild = current;
				out->installed = portagedb_resolve_installed(pkg->parent->parent->database, atom, current->slot);
				
				out->action = PORTAGE_NEW;
				
				if (!out->installed)
					return out;
				
				int cmp = ebuild_installedebuild_cmp(out->ebuild, out->installed);
				if (cmp > 0)
					out->action = PORTAGE_UPDATE;
				else if (cmp == 0)
					out->action = PORTAGE_REPLACE;
				else if (cmp < 0)
					out->action = PORTAGE_DOWNGRADE;
				
				return out;
			}
		}
	}
	
	return NULL;
}

SelectedEbuild* pd_check_selected(Vector* selected, SelectedEbuild* se) {
	for (int i = 0; i < selected->n; i++) {
		SelectedEbuild* current = vector_get(selected, i);
		
		if (current->ebuild->parent != se->ebuild->parent)
			continue;
		
		/* Short circuit if the same ebuild */
		if (current->ebuild == se->ebuild) {
			return current;
		}
		
		if (strcmp(current->ebuild->slot, se->ebuild->slot) == 0) {
			return current;
		}
	}
	
	return NULL;
}

void selected_ebuild_free(SelectedEbuild* se) {
	useflag_free(se->useflags);
	useflag_free(se->explicit_flags);
	
	if (se->use_change) {
		for (int i = 0; i < se->use_change->n; i++) {
			selected_ebuild_free(vector_get(se->use_change, i));
		}
		vector_free(se->use_change);
	}
	
	free(se);
}

void selected_ebuild_print(Emerge* em, SelectedEbuild* se) {
	printf("[ ");
	if (se->action & PORTAGE_NEW)
		printf("N");
	else
		printf(" ");
	
	if (se->action & PORTAGE_SLOT)
		printf("S");
	else
		printf(" ");
	
	if (se->action & PORTAGE_REPLACE)
		printf("R");
	else
		printf(" ");
	
	if (se->action & PORTAGE_UPDATE)
		printf("U");
	else if (se->action & PORTAGE_DOWNGRADE)
		printf("D");
	else
		printf(" ");
	
	if (se->action & PORTAGE_USE_FLAG)
		printf("F");
	else
		printf(" ");
	
	if (se->action & PORTAGE_BLOCK)
		printf("B");
	else
		printf(" ");
	
	printf(" ] %s", se->ebuild->ebuild_key);
	
	printf(" ");
	for (UseFlag* use = se->useflags; use; use = use->next) {
		UseFlag* ebuild_use = NULL;
		use_select_t ebuild_use_status = USE_NONE;
		
		if (se->installed)
			ebuild_use = get_use(se->installed->use, use->name);
		
		if (ebuild_use)
			ebuild_use_status = ebuild_use->status;
		else if (!(em->options & EMERGE_VERBOSE))
			continue;
		
		if ((ebuild_use_status != use->status && se->installed) || em->options & EMERGE_VERBOSE) {
			if (use->status == USE_ENABLE)
				printf("\033[1;31m");
			else
				printf("\033[1;34m-");
			printf("%s%s\033[0m ", use->name, ebuild_use_status != use->status && ebuild_use ? "*": "");
		}
	}
	
	if (se->installed) {
		printf(" %s", se->installed->version->full_version);
		if (se->installed->revision != 0)
			printf("-r%d", se->installed->revision);
	}
	
	printf("\n");
}