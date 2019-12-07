//
// Created by atuser on 12/6/19.
//

#ifndef CPORTAGE_RESOLVE_H
#define CPORTAGE_RESOLVE_H

#include <autogentoo/hacksaw/vector.h>
#include "atom.h"

typedef struct __ResolvedEbuild ResolvedEbuild;
//typedef struct __UseChange UseChange;


//struct __UseChange {
//	/* Index of target ResolvedEbuild in parent
//	 * emerge request */
//	Vector* changed_by;
//	UseFlag* change;
//
//	UseChange* next;
//};

struct __ResolvedEbuild {
	ResolvedEbuild* parent;
	
	/* Creates the backtracking iteration */
	ResolvedEbuild* next;
	
	Dependency* selected_by;
	InstalledEbuild* installed;
	Ebuild* ebuild;
	
	UseFlag* useflags;
	UseFlag* explicit_flags;
	
	int action;
	
	/* Index where this resolve appears  */
	int resolve_index;
	
	int unstable_keywords;
};

int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom);
ResolvedEbuild* resolved_ebuild_new(Ebuild* ebuild, P_Atom* atom);
ResolvedEbuild* resolved_ebuild_resolve(ResolveEmerge* em, P_Atom* atom);
Package* package_resolve_atom(ResolveEmerge* em, P_Atom* atom);

#endif //CPORTAGE_RESOLVE_H
