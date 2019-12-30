//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVE_REQUEST_H
#define AUTOGENTOO_RESOLVE_REQUEST_H

#include "../constants.h"
#include <hacksaw/vector.h>
#include <autogentoo/hacksaw/queue.h>

typedef struct __ResolveRequest ResolveRequest;
typedef struct __ResolveAtom ResolveAtom;

struct __ResolveAtom {
	P_Atom* atom;
	ResolveRequest* parent; //!< The pure request associated with this atom
	
	Vector* parent_slots; //!< list of ResolveSlot* that depend on this atom (update on deletion)
};

struct __ResolveRequest {
	ResolveAtom* selected_by;
	Queue* ebuild; //!< list of Ebuild* in order by priority
};

#endif //AUTOGENTOO_RESOLVE_REQUEST_H
