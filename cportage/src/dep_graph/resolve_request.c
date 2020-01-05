//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include "resolve_request.h"
#include "resolve.h"
#include "../portage_log.h"
#include "../package.h"

ResolveAtom* ra_new(ResolveRequest* parent, P_Atom* atom) {
	ResolveAtom* out = malloc(sizeof(ResolveAtom));
	
	out->atom = atom;
	out->parent = parent;
	out->parent_slots = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	
	return out;
}

ResolveRequest* rr_new(Emerge* environ, ResolveRequest* parent, P_Atom* atom) {
	ResolveRequest* out = malloc(sizeof(ResolveRequest));
	
	out->environ = environ;
	out->selected_by = ra_new(out, atom);
	out->ebuilds = NULL;
	
	Package* pkg = package_resolve_atom(environ, atom);
	if (!pkg) {
		portage_die("Invalid package atom %s", atom->key);
		return NULL;
	}
	
	Queue* prior_1 = queue_new(); /* Stable or keyword unmasked */
	Queue* prior_2 = queue_new(); /* Unstable */
	
	for (Ebuild* current = pkg->ebuilds; current; current = current->older) {
		if (!ebuild_match_atom(current, atom))
			continue;
		
		keyword_t accept = KEYWORD_STABLE;
		for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next) {
			if (ebuild_match_atom(current, keyword->atom) && keyword->keywords[environ->target_arch] < accept)
				accept = keyword->keywords[environ->target_arch];
		}
		
		if (current->keywords[environ->target_arch] == KEYWORD_NONE
		   || current->keywords[environ->target_arch] == KEYWORD_BROKEN)
			continue;
		
		if (current->keywords[environ->target_arch] >= accept)
			queue_add(prior_1, current);
		else
			queue_add(prior_2, current);
	}
	
	queue_concat(prior_1, prior_2);
	out->ebuilds = prior_1;
	out->parent = rp_new(environ, pkg);
	
	return out;
}