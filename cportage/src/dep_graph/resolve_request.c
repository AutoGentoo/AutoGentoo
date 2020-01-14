//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include "resolve_request.h"
#include "resolve.h"
#include "../portage_log.h"
#include "../package.h"


int rr_cmp(ResolveRequest* r1, ResolveRequest* r2) {
	return r1 == r2;
}

ResolveRequest* rr_new(Emerge* environ, ResolveRequest* parent, P_Atom* atom, Vector* add_to) {
	ResolveRequest* out = malloc(sizeof(ResolveRequest));
	
	out->environ = environ;
	out->atom = atom;
	out->ebuilds = NULL;
	out->old = queue_new();
	out->parent_slot = NULL;
	
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
	
	out->added_to = add_to;
	out->index = vector_add(out->added_to, out);
	
	return out;
}

Ebuild* rr_current(ResolveRequest* rr) {
	return (Ebuild*)queue_peek(rr->ebuilds);
}

Ebuild* rr_next(ResolveRequest* rr) {
	queue_add(rr->old, queue_pop(rr->ebuilds));
	return rr_current(rr);
}

void rr_reset(ResolveRequest* rr) {
	queue_concat(rr->old, rr->ebuilds);
	
	Queue* temp = rr->ebuilds;
	rr->ebuilds = rr->old;
	rr->old = temp;
}