//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVE_REQUEST_H
#define AUTOGENTOO_RESOLVE_REQUEST_H

#include "../constants.h"
#include "resolved_package.h"
#include <hacksaw/vector.h>
#include <autogentoo/hacksaw/queue.h>

struct __ResolveAtom {
	P_Atom* atom;
	ResolveRequest* parent; //!< The pure request associated with this atom
	
	Vector* parent_slots; //!< list of ResolveSlot* that depend on this atom (update on deletion)
};

struct __ResolveRequest {
	Emerge* environ;
	ResolvedPackage* parent;
	
	ResolveAtom* selected_by;
	Queue* ebuilds; //!< list of Ebuild* in order by priority
};

ResolveAtom* ra_new(ResolveRequest* parent, P_Atom* atom);
ResolveRequest* rr_new(Emerge* environ, ResolveRequest* parent, P_Atom* atom);


#endif //AUTOGENTOO_RESOLVE_REQUEST_H
