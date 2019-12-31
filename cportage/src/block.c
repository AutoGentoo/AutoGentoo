//
// Created by atuser on 12/16/19.
//

#include "block.h"
#include "dep_graph/cmp.h"

int resolved_ebuild_blocked(ResolvedEbuild* e, P_Atom* block_atom, UseFlag* use) {
	if (!ebuild_match_atom(e->target, block_atom))
		return 0;
	
	AtomFlag* af = NULL;
	for (af = block_atom->useflags; af; af = af->next) {
		UseFlag* uf = use_get(use, af->name);
		if (!uf)
			continue;
		if (af->option == ATOM_USE_ENABLE) {
			if (uf->status != USE_ENABLE)
				return 0;
		}
		else if (af->option == ATOM_USE_DISABLE) {
			if (uf->status != USE_DISABLE)
				break;
		}
		else
			portage_die("Invalid blocker atom flag");
	}
	
	return 1;
}