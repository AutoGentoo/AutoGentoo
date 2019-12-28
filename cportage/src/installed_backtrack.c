//
// Created by atuser on 10/29/19.
//

#include "installed_backtrack.h"
#include "database.h"
#include "dependency.h"
#include <string.h>
#include <errno.h>

void installed_backtrack_rebuild(Emerge* em, ResolvedPackage* se) {
	if (!se->current_slot->current->installed)
		return; /* No need to rebuilt, no one has this as a dep */
	
	for (int i = 0; i < se->current_slot->current->installed->rebuild_depend->n; i++) {
		RebuildEbuild* current_rebuild = vector_get(se->current_slot->current->installed->rebuild_depend, i);
		
		int rebuild = 0;
		char* installed_slot = se->current_slot->current->installed->slot;
		char* new_slot = se->current_slot->current->ebuild->slot;
		
		if (strcmp(installed_slot, new_slot) != 0)
			rebuild = 1;
		
		installed_slot = se->current_slot->current->installed->sub_slot;
		new_slot = se->current_slot->current->ebuild->sub_slot;
		
		/* If one subslot is NULL and the other is not */
		if ((!new_slot || !installed_slot) && new_slot != installed_slot) {
			plog_error("new_slot is %s while installed_slot is %s", se->current_slot->current->ebuild->slot, installed_slot);
			portage_die("Ebuilds subslot is NULL");
		}
		
		if (!rebuild)
			continue;
		
		//ResolvedPackage* se_rebuild = dependency_resolve_ebuild(em, se, current_rebuild->selector);
	}
}

Dependency* prv_installed_backtrack_check_dep(PortageDB* db, Dependency* dep) {
	int resolved = 0;
	Dependency* out = NULL;
	
	if (dep->depends == HAS_DEPENDS && strcmp(dep->target, "||") == 0) {
		/* Check for only one */
		for (Dependency* curr = dep->selectors; curr; curr = curr->next) {
			Dependency* res_dep = prv_installed_backtrack_check_dep(db, curr);
			
			/* Something already selected */
			if (res_dep && resolved) {
				errno = 0;
				plog_error("Multiple depends fuffilled when only one required");
			}
			
			resolved++;
		}
		
		if (resolved > 1)
			return NULL;
	}
	else if (dep->depends == HAS_DEPENDS) {
		for (Dependency* curr = dep->selectors; curr; curr = curr->next) {
			Dependency* res_dep = prv_installed_backtrack_check_dep(db, curr);
			if (!res_dep)
				break;
		}
	}
	else {
	
	}
	
	return out;
}

void prv_installed_backtrack_resolve_dep(PortageDB* db, InstalledEbuild* ebuild, Dependency* dep) {
	for (Dependency* curr = dep; curr; curr = curr->next) {
		if (curr->depends == HAS_DEPENDS) {
			if ((curr->selector == USE_DISABLE || curr->selector == USE_ENABLE)) {
				UseFlag* u = use_get(ebuild->use, curr->target);
				if (u && u->status == curr->selector) {
					prv_installed_backtrack_resolve_dep(db, ebuild, dep->selectors);
				}
			}
			
			continue;
		}
		
		if (curr->atom->blocks != ATOM_BLOCK_NONE) {
			vector_add(db->blockers, curr);
			continue;
		}
		
		InstalledPackage* pkg = map_get(db->installed, curr->atom->key);
		if (!pkg) {
			errno = 0;
			plog_error("Package %s depends on %s but it's not installed", ebuild->parent->key, curr->atom->key);
			continue;
			//exit(1);
		}
		
		InstalledEbuild* curr_inst;
		for (curr_inst = pkg->installed; curr_inst; curr_inst = curr_inst->older_slot) {
			if (curr->atom->slot && strcmp(curr_inst->slot, curr->atom->slot) != 0)
				continue;
			
			InstalledBacktrack* bt = installed_backtrack_new(ebuild, curr);
			vector_add(curr_inst->required_by, bt);
		}
	}
}

void installed_backtrack_resolve(PortageDB* db, InstalledEbuild* ebuild) {
	prv_installed_backtrack_resolve_dep(db, ebuild, ebuild->rdepend);
}

InstalledBacktrack* installed_backtrack_new(InstalledEbuild* required_by, Dependency* selected_by) {
	InstalledBacktrack* out = malloc(sizeof(InstalledBacktrack));
	
	out->required_by = required_by;
	out->selected_by = selected_by;
	
	return out;
}

void backtrack_free(Backtrack* bt) {
	free(bt);
}

void installed_backtrack_rebuild_search(PortageDB* db, InstalledEbuild* parent, Dependency* deptree, rebuild_t type) {
	if (!deptree)
		return;
	
	for (Dependency* curr = deptree; curr; curr = curr->next) {
		installed_backtrack_rebuild_search(db, parent, curr->selectors, type);
		if (curr->depends == IS_ATOM && curr->atom->sub_opts == ATOM_SLOT_REBUILD)
			installed_backtrack_rebuild_new(db, parent, curr, type);
	}
}

void installed_backtrack_rebuild_new(PortageDB* db, InstalledEbuild* rebuild, Dependency* dep, rebuild_t type) {
	RebuildEbuild* backtrack = malloc(sizeof(RebuildEbuild));
	backtrack->rebuild = rebuild;
	backtrack->selector = dep;
	backtrack->type = type;
	
	vector_add(db->rebuilds, backtrack);
}

void installed_backtrack_rebuild_resolve(PortageDB* db, rebuild_t types) {
	for (int i = 0; i < db->rebuilds->n; i++) {
		RebuildEbuild* backtrack = (RebuildEbuild*)vector_get(db->rebuilds, i);
		if (!(backtrack->type & types))
			continue;
		
		/* Get the parent  */
		InstalledPackage* package = map_get(db->installed, backtrack->selector->atom->key);
		if (!package) {
			plog_debug("Invalid backtracking request for package %s (%s)", backtrack->rebuild->parent->key, backtrack->selector->atom->key);
			plog_debug("Is %s a binary package?", backtrack->rebuild->parent->key);
			continue;
		}
		
		InstalledEbuild* curr;
		for (curr = package->installed; curr; curr = curr->older_slot) {
			if (backtrack->selector->atom->slot && strcmp(curr->slot, backtrack->selector->atom->slot) != 0)
				continue;
			
			if (backtrack->type == EBUILD_REBUILD_DEPEND)
				vector_add(curr->rebuild_depend, backtrack);
			if (backtrack->type == EBUILD_REBUILD_RDEPEND)
				vector_add(curr->rebuild_rdepend, backtrack);
		}
	}
}

void installed_backtrack_rebuild_free(RebuildEbuild* rebuild) {
	free(rebuild);
}

