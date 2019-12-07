//
// Created by atuser on 12/4/19.
//

#include <stdio.h>
#include <stdlib.h>
#include "dep_v4.h"
#include "package.h"

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
		
		for (SelectedEbuild* selected_by = parent_ebuild; selected_by; selected_by = selected_by->parent_ebuild)
			selected_ebuild_print(emerge, selected_by);
		
		if (out->use_change) {
			printf("Use flags change by following dependencies");
			for (int i = 0; i < out->use_change->n; i++)
				selected_ebuild_print(emerge, vector_get(out->use_change, i));
		}
		
		portage_die("Invalid required use");
	}
}

Backtrack* backtrack_new(ResolvedEbuild* ebuild, Package* backtrack_to, backtrack_t action) {
	Backtrack* out = malloc(sizeof(Backtrack));
	
	out->failed = ebuild;
	out->backtrack_to = backtrack_to;
	out->action = action;
	
	return out;
}

Backtrack* dependency_resolve_ebuild(ResolveEmerge* emerge, ResolvedEbuild* selected_by, Dependency* dep, ResolvedEbuild** resolve_dest) {
	ResolvedEbuild* matching_ebuilds = resolved_ebuild_resolve(emerge, dep->atom);
	
	if (!matching_ebuilds) {
		//return backtrack_new(matching_ebuilds, )
	}
}