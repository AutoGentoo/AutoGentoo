//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVED_PACKAGE_H
#define AUTOGENTOO_RESOLVED_PACKAGE_H

#include <autogentoo/hacksaw/vector.h>
#include "../constants.h"
#include "resolve_request.h"

typedef struct __ResolvedPackage ResolvedPackage;

struct __ResolvedPackage {
	Package* parent;
	
	Vector* selected_slots; //!< Slots waiting to be installed
	Vector* requests; //!< Unmerged pure requests
};

ResolvedPackage* rp_new(Package* parent);

void rp_merge(ResolvedPackage* rp, ResolveRequest* rr);

#endif //AUTOGENTOO_RESOLVED_PACKAGE_H
