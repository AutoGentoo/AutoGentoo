//
// Created by atuser on 12/4/19.
//

#ifndef AUTOGENTOO_DEP_V4_H
#define AUTOGENTOO_DEP_V4_H

typedef struct __ResolvedEbuild ResolvedEbuild;
typedef struct __ResolvedEbuild ResolvedEbuild;
typedef struct __ResolveEmerge ResolveEmerge;

#include <autogentoo/hacksaw/vector.h>
#include "atom.h"
#include "constants.h"

typedef enum {
	BACKTRACK_NONE,
	BACKTRACK_VERSION,
	BACKTRACK_USE
} backtrack_t;

struct __UseChange {
	/* Index of target ResolvedEbuild in parent
	 * emerge request */
	int resolve_index;
	
};

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
	
	/*
	 * A set of use flag changes if action == BACKTRACK_USE
	 */
	UseFlag* use_changes;
};

struct __ResolvedEbuild {
	ResolvedEbuild* parent;
	ResolvedEbuild* next; /* Used for backtracking if this */
	
	Dependency* selected_by;
	InstalledEbuild* installed;
	Ebuild* ebuild;
	
	
	UseFlag* useflags;
	UseFlag* explicit_flags;
	
	int action;
	
	/* Index where this resolve appears  */
	int resolve_index;
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

Backtrack* dependency_resolve_ebuild(ResolveEmerge* emerge, ResolvedEbuild* selected_by, Dependency* dep, ResolvedEbuild** resolve_dest);
Backtrack* dependency_resolve(ResolveEmerge* emerge, Dependency* dependency);



#endif //AUTOGENTOO_DEP_V4_H
