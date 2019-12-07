//
// Created by atuser on 12/4/19.
//

#ifndef AUTOGENTOO_DEP_V4_H
#define AUTOGENTOO_DEP_V4_H

typedef struct __ResolveEmerge ResolveEmerge;

#include <autogentoo/hacksaw/vector.h>
#include "atom.h"
#include "constants.h"
#include "resolve.h"

typedef enum {
	BACKTRACK_NONE,
	BACKTRACK_VERSION,
	BACKTRACK_USE
} backtrack_t;

struct __Backtrack {
	/* Comes from the stack frame where
	 * the dependency resolve failed */
	ResolvedEbuild* failed;
	
	/* Keep bringing up failure
	 * until stack frame resolving this package
	*/
	Package* backtrack_to;
	
	/*
	 * Should we try a different version or
	 * a different use flag configuration
	 */
	backtrack_t action;
};



struct __ResolveEmerge {
	/*
	 * When an ebuild is selected for emerge,
	 * it is immediatly added here. This means
	 * that ebuild will show up before their
	 * dependencies and therefore should not be
	 * emerged this way
	 */
	Vector* resolved_ebuilds;
	
	/*
	 * This vector is different from the above in that
	 * it is order as such:
	 *
	 * DEP, DEP, DEP, PACKAGE
	 * instead of:
	 * PACKAGE, DEP, DEP, DEP
	 *
	 * This is the way packages should be emerges
	 */
	Vector* dependency_order;
	
	Emerge* parent;
};

Backtrack* backtrack_new(ResolvedEbuild* ebuild, Package* backtrack_to, backtrack_t action);
Backtrack* dependency_resolve_ebuild(ResolveEmerge* emerge, ResolvedEbuild* selected_by, Dependency* dep, ResolvedEbuild** resolve_dest);
Backtrack* dependency_resolve(ResolveEmerge* emerge, Dependency* dependency);



#endif //AUTOGENTOO_DEP_V4_H
