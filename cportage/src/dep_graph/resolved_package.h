//
// Created by atuser on 12/30/19.
//

#ifndef AUTOGENTOO_RESOLVED_PACKAGE_H
#define AUTOGENTOO_RESOLVED_PACKAGE_H

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "../constants.h"


struct __ResolvedPackage {
	Emerge* environ;
	Package* parent;
	
	SmallMap* selected_slots; //!< Slots waiting to be installed
	Vector* requests; //!< Unmerged pure requests
};

ResolvedPackage* rp_get(Emerge* environ, char* key);
ResolvedPackage* rp_new(Emerge* environ, Package* parent);
int rp_merge(ResolvedPackage* rp, ResolveRequest* rr);

#endif //AUTOGENTOO_RESOLVED_PACKAGE_H
