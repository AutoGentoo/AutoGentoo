//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include "resolved_package.h"
#include "../emerge.h"
#include "../package.h"
#include "resolved_slot.h"
#include "resolve_request.h"

ResolvedPackage* rp_get(Emerge* environ, char* key) {
	return map_get(environ->selected, key);
}

ResolvedPackage* rp_new(Emerge* environ, Package* parent) {
	ResolvedPackage* try = rp_get(environ, parent->key);
	if (try)
		return try;
	
	ResolvedPackage* out = malloc(sizeof(ResolvedPackage));
	
	out->environ = environ;
	out->parent = parent;
	out->requests = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	out->selected_slots = small_map_new(5);
	
	return out;
}

int prv_rp_merge_slot(ResolvedSlot* rs, ResolveRequest* rr) {

}

int rp_merge(ResolvedPackage* rp, ResolveRequest* rr) {
	ResolvedSlot* rs_sel = small_map_get(rp->selected_slots, rr_current(rr)->slot);
	if (rs_sel)
		return prv_rp_merge_slot(rs_sel, rr);
	
	
}