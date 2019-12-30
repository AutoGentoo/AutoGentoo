//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_REQUIRE_USE_H
#define AUTOGENTOO_REQUIRE_USE_H

#include <autogentoo/hacksaw/vector.h>
#include "constants.h"
#include "atom.h"
#include "cportage_defines.h"

struct __PackageUse {
	P_Atom* atom;
	UseFlag* flags;
	keyword_t keyword_required; // At least this keyword must be present to apply (package.use.stable)
};

struct __UseFlag {
	char* name;
	use_t status; // Only USE_DISABLE and USE_ENABLE
	use_priority_t priority;
	UseFlag* next;
};

struct __RequiredUse {
	char* target;
	use_t option;
	RequiredUse* depend;
	RequiredUse* next;
};

/**
 * Generate a new useflag with these settings
 * @param name name of the new flag
 * @param status USE_ENABLE or USE_DISABLE
 * @param priority set the write level
 * @return the new flag
 */
UseFlag* use_new(char* name, use_t status, use_priority_t priority);

/**
 * Parse the iuse from the repo metadata cache
 * @param em parent instance
 * @param metadata the IUSE line in the repo metadata
 * @return the new flag
 */
UseFlag* use_iuse_parse(Emerge* em, char* metadata);

/**
 * Set a useflag to new_val
 * @param head use flag to start searching
 * @param use_search the target use flag name
 * @param new_val the new value
 * @param priority write priority
 * @return the old value
 */
use_t use_set(UseFlag* head, char* use_search, use_t new_val, use_priority_t priority);

/**
 * Get a use flag from a list
 * @param head the head of the linked list
 * @param useflag the target useflag name
 * @return found useflag, NULL if not found
 */
UseFlag* use_get(UseFlag* head, char* useflag);

/**
 * Free a list of flags
 * @param head the head of the list
 */
void use_free(UseFlag* head);


RequiredUse* use_build_required_use(char* target, use_t option);
int ebuild_check_required_use(RequiredUse* ru);
void requireduse_free(RequiredUse* ptr);
AtomFlag* dependency_useflag(Ebuild* resolved, AtomFlag* new_flags, AtomFlag* old_flags);

void useflag_parse(FILE* fp, Vector* useflags, keyword_t keyword_required, use_priority_t priority);
void emerge_parse_useflags(Emerge* emerge);
void emerge_apply_package_use(Emerge* emerge);

#endif //AUTOGENTOO_REQUIRE_USE_H
