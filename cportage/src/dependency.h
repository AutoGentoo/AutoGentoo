//
// Created by atuser on 12/4/19.
//

#ifndef AUTOGENTOO_DEP_V4_H
#define AUTOGENTOO_DEP_V4_H

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

Backtrack* backtrack_new(ResolvedEbuild* ebuild, Package* backtrack_to, backtrack_t action);

/**
 * Resolve an atom to a specific ebuild
 * @param emerge parent emerge environment
 * @param selected_by parent ebuild selection
 * @param dep dependency selection
 * @return resolved ebuild (NULL if no need to add to vector)
 */
ResolvedEbuild* dependency_resolve_ebuild(Emerge* emerge, ResolvedEbuild* selected_by, Dependency* dep);

/**
 * Raise an error if a blocked package is selected
 * Marked blocked ebuilds as blocked
 * @param emerge the parent environment
 * @param atom the blocking atom
 */
void dependency_resolve_block(Emerge* emerge, ResolvedEbuild* blocker, P_Atom* atom);

/**
 * Main protocol to perform dependency resolution
 * All dependencies will be placed into target vector
 * @param emerge parent emerge environment
 * @param parent current ebuild for use flag checks
 * @param dependency the depend expression to iterate through
 * @param add_to where to add the resolved ebuilds to
 */
void dependency_resolve(Emerge* emerge, ResolvedEbuild* parent, Dependency* dependency, Vector* add_to);

/**
 * Return the first match to an ebuild in the
 * the search vector
 *
 * This is a recursive call, for the first
 * use emerge->selected for search
 *
 * This will only check the slot
 * @param search the vector of ResolvedEbuilds, will also check deps
 * @param check the slot to attempt to match
 * @return the first matched selected ebuild
 */
ResolvedEbuild* dependency_check_selected(Vector* search, ResolvedEbuild* check);

/**
 * Perform an in-order traversal of every dependency in traverse
 * and add to target
 * @param traverse a vector of tree heads
 * @param target vector to build
 */
void dependency_build_vector(Vector* traverse, Vector* target);

#endif //AUTOGENTOO_DEP_V4_H
