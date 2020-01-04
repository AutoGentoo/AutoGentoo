//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVED_SLOT_H
#define AUTOGENTOO_RESOLVED_SLOT_H

#include <autogentoo/hacksaw/vector.h>

typedef struct __ResolvedSlot ResolvedSlot;

struct __ResolvedSlot {
	
	Vector* parent_requests; //!< ResolveRequest merge to make this slot
	
};

#endif //AUTOGENTOO_RESOLVED_SLOT_H
