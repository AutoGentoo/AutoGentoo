//
// Created by atuser on 10/29/19.
//

#include "backtrack.h"
#include "dependency.h"
#include "database.h"
#include <string.h>
#include <errno.h>

void backtrack_rebuild(Emerge* em, SelectedEbuild* se, Vector* dependency_ordered, Vector* dependency_selected,
                       Vector* dependency_blocks) {
	if (!se->installed)
		return; /* No need to rebuilt, no one has this as a dep */
	
	for (int i = 0; i < se->installed->rebuild_depend->n; i++) {
		RebuildEbuild* current_rebuild = vector_get(se->installed->rebuild_depend, i);
		
		int rebuild = 0;
		char* installed_slot = se->installed->slot;
		char* new_slot = se->ebuild->slot;
		
		if (strcmp(installed_slot, new_slot) != 0)
			rebuild = 1;
		
		installed_slot = se->installed->sub_slot;
		new_slot = se->ebuild->sub_slot;
		
		/* If one subslot is NULL and the other is not */
		if ((!new_slot || !installed_slot) && new_slot != installed_slot) {
			plog_error("new_slot is %s while installed_slot is %s", se->ebuild->slot, installed_slot);
			portage_die("Ebuilds subslot is NULL");
		}
		
		if (!rebuild)
			continue;
		
		SelectedEbuild* se_rebuild = pd_resolve_single(em, se, current_rebuild->selector, dependency_selected);
		
	}
}

Dependency* prv_backtrack_check_dep(PortageDB* db, Dependency* dep) {
	int resolved = 0;
	Dependency* out = NULL;
	
	if (dep->depends == HAS_DEPENDS && strcmp(dep->target, "||") == 0) {
		/* Check for only one */
		
		for (Dependency* curr = dep->selectors; curr; curr = curr->next) {
			Dependency* res_dep = prv_backtrack_check_dep(db, curr);
			
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
			Dependency* res_dep = prv_backtrack_check_dep(db, curr);
			if (!res_dep)
				break;
		}
	}
	else {
	
	}
	
	return out;
}

void prv_backtrack_resolve_dep(PortageDB* db, InstalledEbuild* ebuild, Dependency* dep) {
	for (Dependency* curr = dep; curr; curr = curr->next) {
		if (curr->depends == HAS_DEPENDS) {
			if ((curr->selector == USE_DISABLE || curr->selector == USE_ENABLE)) {
				UseFlag* u = use_get(ebuild->use, curr->target);
				if (u && u->status == curr->selector) {
					prv_backtrack_resolve_dep(db, ebuild, dep->selectors);
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
			
			Backtrack* bt = backtrack_new(ebuild, curr);
			vector_add(curr_inst->required_by, bt);
		}
	}
}

void backtrack_resolve(PortageDB* db, InstalledEbuild* ebuild) {
	prv_backtrack_resolve_dep(db, ebuild, ebuild->rdepend);
}

Backtrack* backtrack_new(InstalledEbuild* required_by, Dependency* selected_by) {
	Backtrack* out = malloc(sizeof(Backtrack));
	
	out->required_by = required_by;
	out->selected_by = selected_by;
	
	return out;
}

void backtrack_free(Backtrack* bt) {
	free(bt);
}

void backtrack_rebuild_search(PortageDB* db, InstalledEbuild* parent, Dependency* deptree, rebuild_t type) {
	if (!deptree)
		return;
	
	for (Dependency* curr = deptree; curr; curr = curr->next) {
		backtrack_rebuild_search(db, parent, curr->selectors, type);
		if (curr->depends == IS_ATOM && curr->atom->sub_opts == ATOM_SLOT_REBUILD)
			backtrack_rebuild_new(db, parent, curr, type);
	}
}

void backtrack_rebuild_new(PortageDB* db, InstalledEbuild* rebuild, Dependency* dep, rebuild_t type) {
	RebuildEbuild* backtrack = malloc(sizeof(RebuildEbuild));
	backtrack->rebuild = rebuild;
	backtrack->selector = dep;
	backtrack->type = type;
	
	vector_add(db->rebuilds, backtrack);
}

void backtrack_rebuild_resolve(PortageDB* db, rebuild_t types) {
	for (int i = 0; i < db->rebuilds->n; i++) {
		RebuildEbuild* backtrack = (RebuildEbuild*)vector_get(db->rebuilds, i);
		if (!(backtrack->type & types))
			continue;
		
		/* Get the parent  */
		InstalledPackage* package = map_get(db->installed, backtrack->selector->atom->key);
		if (!package) {
			plog_warn("Invalid backtracking request for package %s (%s)", backtrack->rebuild->parent->key, backtrack->selector->atom->key);
			plog_warn("Is %s a binary package?", backtrack->rebuild->parent->key);
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

void backtrack_rebuild_free(RebuildEbuild* rebuild) {
	free(rebuild);
}

