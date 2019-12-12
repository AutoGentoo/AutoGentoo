//
// Created by atuser on 12/6/19.
//

#ifndef CPORTAGE_RESOLVE_H
#define CPORTAGE_RESOLVE_H

#include <autogentoo/hacksaw/vector.h>
#include "atom.h"

typedef struct __ResolvedEbuild ResolvedEbuild;
typedef struct __SeletecBy SelectedBy;
typedef struct __ResolvedPackage ResolvedPackage;


typedef enum {
	PORTAGE_NEW = 1 << 0, //!< New package
	PORTAGE_SLOT = 1 << 1, //!< Install side by side
	PORTAGE_REMOVE = 1 << 2, //!< Remove package 'old'
	PORTAGE_UPDATE = 1 << 3, //!< Install a new package and remove the old one
	PORTAGE_DOWNGRADE = 1 << 4,
	PORTAGE_REPLACE = 1 << 5,
	PORTAGE_USE_FLAG = 1 << 6,
	PORTAGE_BLOCK = 1 << 7
} dependency_t;

struct __SelectedBy {
	ResolvedPackage* parent;
	Dependency* selected_by;
};

struct __ResolvedPackage {
	Emerge* environ;
	
	ResolvedEbuild* ebuilds;
	ResolvedEbuild* current;
	
	Vector* parents;
	
	Vector* pre_dependency;
	Vector* post_dependency;
};

struct __ResolvedEbuild {
	ResolvedPackage* parent;
	
	/* Creates the backtracking iteration */
	ResolvedEbuild* next;
	ResolvedEbuild* back;
	
	InstalledEbuild* installed;
	Ebuild* ebuild;
	
	UseFlag* useflags;
	UseFlag* explicit_flags;
	
	dependency_t action;
	int unstable_keywords;
};

int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom);
ResolvedEbuild* resolved_ebuild_new(Ebuild* ebuild, P_Atom* atom);
ResolvedPackage* resolved_ebuild_resolve(Emerge* em, P_Atom* atom);
Package* package_resolve_atom(Emerge* em, P_Atom* atom);
void resolved_ebuild_free(ResolvedEbuild* ptr);

int resolved_ebuild_is_blocked(Emerge*, ResolvedEbuild*);
int resolved_ebuild_use_build(ResolvedEbuild* parent, ResolvedEbuild* out, P_Atom* atom);

#endif //CPORTAGE_RESOLVE_H
