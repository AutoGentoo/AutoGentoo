//
// Created by atuser on 12/30/19.
//

#include <stdlib.h>
#include "resolved_package.h"
#include "../emerge.h"
#include "../package.h"

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
	out->selected_slots = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	
	return out;
}