//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVE_REQUEST_H
#define AUTOGENTOO_RESOLVE_REQUEST_H

#include "../constants.h"
#include "resolved_package.h"
#include "resolved_slot.h"
#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/queue.h>
#include <autogentoo/hacksaw/stack.h>
#include <autogentoo/hacksaw/set.h>

struct __ResolveRequest {
	Emerge* environ;
	ResolvedPackage* parent;
	
	P_Atom* atom;
	Queue* ebuilds; //!< list of Ebuild* in order by priority
	Queue* old;
	
	//Set* parent_slots; //!< list of ResolveSlot* that depend on this atom (update on deletion)
	ResolvedSlot* parent_slot;
	
	Vector* added_to;
	int index;
};

int rr_cmp(ResolveRequest* r1, ResolveRequest* r2);
ResolveRequest* rr_new(Emerge* environ, ResolveRequest* parent, P_Atom* atom, Vector* add_to);
Ebuild* rr_current(ResolveRequest* rr);
Ebuild* rr_next(ResolveRequest* rr);
void rr_reset(ResolveRequest* rr);
void rr_free(ResolveRequest* rr);


#endif //AUTOGENTOO_RESOLVE_REQUEST_H
