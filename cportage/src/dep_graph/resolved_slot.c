//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include <autogentoo/hacksaw/set.h>
#include "resolved_slot.h"
#include "resolve_request.h"


ResolvedSlot* rs_new(ResolvedPackage* rp) {
	ResolvedSlot* out = malloc(sizeof(ResolvedSlot));
	out->parent = rp;
	
	out->target = NULL;
	out->parent_requests = set_new((element_cmp)rr_cmp);
	
	return out;
}

int rs_remove(ResolvedSlot* rs, ResolveRequest* rr) {
	if (!rs)
		return 1;
	
	return 0;
}

int rs_add(ResolvedSlot* rs, ResolveRequest* rr) {
	if (!rs_remove(rr->parent_slot, rr))
		return 0;
	
	rr->parent_slot = rs;
	set_add(rs->parent_requests, rr);
	
}

void rs_free(ResolvedSlot rs) {

}