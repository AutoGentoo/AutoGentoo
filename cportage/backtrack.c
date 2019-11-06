//
// Created by atuser on 10/29/19.
//

#include "backtrack.h"
#include "dependency.h"
#include "database.h"

void backtrack_selected_ebuild(Emerge* em, Vector* dependency_ordered, SelectedEbuild* se) {
	if (!se->installed)
		return; /* No need to rebuilt, no one has this as a dep */
	
	
}