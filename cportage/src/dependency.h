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
 * @param res pointer to resolved ebuild (NULL if no need to add to vector)
 * @return 0 if failure, 1 if found 2 if seleted
 */
int dependency_resolve_ebuild(Emerge* emerge, ResolvedPackage* selected_by, Dependency* dep, ResolvedPackage** res);

/**
 * Raise an error if a blocked package is selected
 * Marked blocked ebuilds as blocked
 * @param emerge the parent environment
 * @param atom the blocking atom
 * @return whether or not the block was resolved (portage_die() if not)
 */
int dependency_resolve_block(Emerge* emerge, ResolvedPackage* blocker, P_Atom* atom);

/**
 * Add all the dependencies to the child vectors
 * pre = [BDEPEND, DEPEND, RDEPEND]
 * post = [POST]
 * @param emerge parent emerge environment
 * @param parent ebuild to add dependencies from
 * @param add_to vector to add this ebuild to (NULL if already added)
 * @param atom_stack_str log info
 */
int dependency_sub_resolve(Emerge* emerge, ResolvedPackage* parent, char* atom_stack_str);

/**
 * Main protocol to perform dependency resolution
 * All dependencies will be placed into target vector
 * @param emerge parent emerge environment
 * @param parent current ebuild for use flag checks
 * @param dependency the depend expression to iterate through
 * @param add_to where to add the resolved ebuilds to
 * @return whether or not to stop resolving (could have been re-resolved inside)
 */
int dependency_resolve(Emerge* emerge, ResolvedPackage* parent, Dependency* dependency, Vector* add_to);

/**
 * Perform an in-order traversal of every dependency in traverse
 * and add to target
 * @param traverse a vector of tree heads
 * @param target vector to build
 */
void dependency_build_vector(Vector* traverse, Vector* target);

int dependency_check_blocked(Emerge* em, ResolvedEbuild* check);

#endif //AUTOGENTOO_DEP_V4_H
