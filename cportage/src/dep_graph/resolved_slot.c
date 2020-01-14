//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include <autogentoo/hacksaw/set.h>
#include <autogentoo/hacksaw/queue_set.h>
#include "resolved_slot.h"
#include "resolve_request.h"
#include "../portage_log.h"
#include "resolve.h"


ResolvedSlot* rs_new(ResolvedPackage* rp) {
	ResolvedSlot* out = malloc(sizeof(ResolvedSlot));
	out->parent = rp;
	
	out->target = NULL;
	out->parent_requests = set_new((element_cmp)rr_cmp);
	
	out->delete_on_re_free = 1;
	
	return out;
}

int rs_remove(ResolvedSlot* rs, ResolveRequest* rr) {
	if (!rs)
		return 1;
	
	set_remove(rs->parent_requests, rr);
	
	
	return 0;
}

int rs_add(ResolvedSlot* rs, ResolveRequest* rr) {
	if (!rs_remove(rr->parent_slot, rr))
		return 0;
	
	rr->parent_slot = rs;
	set_add(rs->parent_requests, rr);
	
	if (rs->target) {
		portage_die("RS already has a target rs_add() - %s:%d", __FILE__, __LINE__);
	}
	
	return 1;
}

int rs_generate(ResolvedSlot* rs) {
	QueueSet* matching_ebuilds = queue_set_new(NULL);
	
	for (int i = 0; i < rs->parent_requests->parent->n; i++) {
		ResolveRequest* rr_i = set_get(rs->parent_requests, i);
		
		for (struct queue_Node* ebuild_node = rr_i->ebuilds->head; ebuild_node; ebuild_node = ebuild_node->next) {
			for (int j = 0; i < rs->parent_requests->parent->n; j++) {
				if (i == j)
					continue;
				
				ResolveRequest* rr_j = set_get(rs->parent_requests, j);
				if (ebuild_match_atom((Ebuild*)ebuild_node->data, rr_j->atom))
					queue_set_add(matching_ebuilds, ebuild_node->data);
			}
		}
	}
	
	while (queue_set_peek(matching_ebuilds)) {
		Ebuild* resolved_ebuild = queue_set_pop(matching_ebuilds);
		if (!resolved_ebuild) {
			portage_die("invalid resolved ebuild NULL rs_generate() - %s:%d", __FILE__, __LINE__);
			return 0;
		}
		
		
	}
	
	return 0;
}

void rs_free(ResolvedSlot* rs) {
	if (rs->target)
		re_free(rs->target);
	
	
}