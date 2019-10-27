//
// Created by atuser on 10/27/19.
//

#ifndef AUTOGENTOO_CONFLICT_H
#define AUTOGENTOO_CONFLICT_H

#include "constants.h"

/**
 * Iter through explicit flags from use1 and use2
 * @param use1 first list to go through (previous selection)
 * @param use2 current selection
 * @return NULL if no error, UseFlag* of current selection where error occured
 */
void conflict_use_check(UseFlag* use1, UseFlag* use2, UseFlag** conflict1, UseFlag** conflict2);

/**
 * Resolve a use conflict and return a list of suggested use edits
 * A dependency resolve needs to run again in this case
 * @param conflict_prev the use flag to attempt to change
 * @param target_val try to set useflag to this value
 * @return a list of suggestions, NULL if failed
 */
Suggestion* conflict_use_resolve(UseFlag* conflict_prev, use_t target_val);

/**
 * Free a list of suggestions
 * @param s
 */
void suggestion_free(Suggestion* s);

#endif //AUTOGENTOO_CONFLICT_H
