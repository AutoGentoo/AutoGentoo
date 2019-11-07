//
// Created by atuser on 10/29/19.
//

#include "backtrack.h"
#include "dependency.h"
#include "database.h"
#include <string.h>

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
	
	Dependency* next;
	while (deptree) {
		next = deptree->next;
		backtrack_rebuild_search(db, parent, deptree->selectors, type);
		if (deptree->depends == IS_ATOM && deptree->atom->sub_opts == ATOM_SLOT_REBUILD)
			backtrack_rebuild_new(db, parent, deptree, type);
		deptree = next;
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

