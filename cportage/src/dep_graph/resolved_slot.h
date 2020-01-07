//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVED_SLOT_H
#define AUTOGENTOO_RESOLVED_SLOT_H

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/set.h>
#include "resolved_ebuild.h"
#include "resolved_package.h"

typedef struct __ResolvedSlot ResolvedSlot;

struct __ResolvedSlot {
	ResolvedPackage* parent;
	
	ResolvedEbuild* target;
	Set* parent_requests; //!< ResolveRequest merge to make this slot
};

ResolvedSlot* rs_new(ResolvedPackage* rp);
int rs_add(ResolvedSlot* rs, ResolveRequest* rr);
void rs_free(ResolvedSlot rs);

#endif //AUTOGENTOO_RESOLVED_SLOT_H
