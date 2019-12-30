//
// Created by atuser on 10/27/19.
//

#include <string.h>
#include <stdlib.h>
#include "use.h"
#include "deprecated/dependency.h"
#include "suggestion.h"
#include "package.h"
#include "conflict.h"

inline void conflict_use_check(UseFlag* use1, UseFlag* use2, UseFlag** conflict1, UseFlag** conflict2) {
	/* Check if the explicit flags from previous and current selections match */
	UseFlag* sel_explicit_key = NULL;
	UseFlag* out_explicit_key = NULL;
	for (sel_explicit_key = use1; sel_explicit_key; sel_explicit_key = sel_explicit_key->next) {
		for (out_explicit_key = use2; out_explicit_key; out_explicit_key = out_explicit_key->next) {
			if (strcmp(sel_explicit_key->name, out_explicit_key->name) == 0 &&
			    sel_explicit_key->status != out_explicit_key->status)
				break;
		}
		
		if (out_explicit_key) /* Error */
			break;
	}
	
	*conflict1 = sel_explicit_key;
	*conflict2 = out_explicit_key;
}

Suggestion* conflict_use_resolve(UseFlag* conflict_prev, use_t target_val) {
	if (!conflict_prev->reason->selected_by)
		return NULL;
	
	Suggestion* out = NULL;
	char* atom_str = atom_get_str(conflict_prev->reason->selected_by->selected_by->atom);
	*(strchr(atom_str, ' ')) = 0;
	
	for (UseReason* current_reason = conflict_prev->reason; current_reason; current_reason = current_reason->next) {
		/**
			ATOM_USE_DISABLE, //!< atom[-bar]
			ATOM_USE_ENABLE, //!< atom[bar]
			ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
			ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
			ATOM_USE_EQUAL, //!< atom[bar=]
			ATOM_USE_OPPOSITE //!< atom[!bar=]
		 */
		
		UseFlag* explicit_search = NULL;
		char* suggest_flag_name = NULL;
		use_t set_to = target_val;
		UseFlag* useflag_search = NULL;
		
		/* This use flag set is not conditional
		 * Must have been selected by a parent depend line
		 * If no parent depend line, we try to stop this package from being pulled in
		*/
		if (current_reason->flag->option == ATOM_USE_DISABLE || current_reason->flag->option == ATOM_USE_ENABLE) {
			/* use? ( expr ) */
			Dependency* parent_dependency = current_reason->selected_by->selected_by;
			if (!parent_dependency) {
				/* We could technically backtrack more
				 * Not going to implement this for now
				 * */
				return NULL;
			}
			
			if (parent_dependency->selector == USE_ENABLE)
				set_to = USE_DISABLE;
			else
				set_to = USE_ENABLE;
			
			explicit_search = rp_current(conflict_prev->reason->selected_by->parent)->explicit_flags;
			useflag_search = rp_current(conflict_prev->reason->selected_by->parent)->useflags;
			suggest_flag_name = parent_dependency->target;
		}
		else if (current_reason->flag->option == ATOM_USE_ENABLE_IF_ON) {
			/* depend[use?] */
			suggest_flag_name = conflict_prev->name;
			explicit_search = rp_current(current_reason->selected_by->parent)->explicit_flags;
			useflag_search = rp_current(conflict_prev->reason->selected_by->parent)->useflags;
		}
		else if (current_reason->flag->option == ATOM_USE_DISABLE_IF_OFF) {
			/* depend[use?] */
			suggest_flag_name = conflict_prev->name;
			explicit_search = rp_current(current_reason->selected_by->parent)->explicit_flags;
			useflag_search = rp_current(current_reason->selected_by->parent)->useflags;
		}
		else if (current_reason->flag->option == ATOM_USE_EQUAL) {
			useflag_search = explicit_search = rp_current(current_reason->selected_by->parent)->useflags;
			suggest_flag_name = conflict_prev->name;
		}
		else if (current_reason->flag->option == ATOM_USE_OPPOSITE) {
			/* Use this to reverse the value */
			set_to = conflict_prev->status;
			useflag_search = explicit_search = rp_current(current_reason->selected_by->parent)->useflags;
			suggest_flag_name = conflict_prev->name;
			
		}
		else {
			portage_die("Invalid depend expression for suggestions");
			return NULL;
		}
		
		UseFlag* target_flag = use_get(explicit_search, suggest_flag_name);
		if (!target_flag) {
			UseFlag* non_explicit = use_get(useflag_search, suggest_flag_name);
			if (non_explicit && non_explicit->priority == PRIORITY_FORCE) {
				/* We cannot change this flag */
				suggestion_free(out);
				return NULL;
			}
			
			// This flag is not explicit, just disable it
			Suggestion* remove_buf = suggestion_new(atom_str, ">=%s %s%s", rp_current(conflict_prev->reason->selected_by->parent)->ebuild->ebuild_key, set_to == USE_ENABLE ? "" : "-", suggest_flag_name);
			remove_buf->next = out;
			out = remove_buf;
			continue; /* Go to next error */
		}
		
		Suggestion* buf = conflict_use_resolve(target_flag, set_to);
		
		if (!buf) {
			suggestion_free(out);
			return NULL;
		}
		
		buf->next = out;
		out = buf;
	}
	
	return out;
}

void suggestion_free(Suggestion* s) {
	Suggestion* temp;
	
	for (Suggestion* c = s; c;) {
		free(c->required_by);
		free(c->line_addition);
		temp = c->next;
		free(c);
		c = temp;
	}
}