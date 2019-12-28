//
// Created by atuser on 12/6/19.
//

#ifndef CPORTAGE_RESOLVE_H
#define CPORTAGE_RESOLVE_H

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/set.h>
#include "atom.h"

typedef struct __ResolvedEbuild ResolvedEbuild;
typedef struct __ResolvedPackage ResolvedPackage;
typedef struct __ResolvedSlot ResolvedSlot;
typedef struct __SelectionRequest SelectionRequest;


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

struct __ResolvedSlot {
	char* slot;
	ResolvedEbuild* head;
	ResolvedEbuild* current;
	
	ResolvedSlot* next;
};

struct __SelectionRequest {
	ResolvedPackage* parent;
	Dependency* selected_by;
	ResolvedSlot* head;
};

struct __ResolvedEbuild {
	ResolvedPackage* parent;
	ResolvedSlot* parent_slot;
	
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

struct __ResolvedPackage {
	Emerge* environ;
	
	Vector* selection_requests;
	ResolvedSlot* current;
	
	Vector* bdepend;
	Vector* depend;
	Vector* rdepend;
	Vector* pdepend;
	
	int remove_index;
	Vector* added_to;
};


int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom);
ResolvedEbuild* resolved_ebuild_new(Ebuild* ebuild, P_Atom* atom);
ResolvedPackage* resolved_ebuild_resolve(Emerge* em, P_Atom* atom);
Package* package_resolve_atom(Emerge* em, P_Atom* atom);
void resolved_ebuild_free(ResolvedEbuild* ptr);

int resolved_ebuild_use_build(ResolvedEbuild* out, Set* update_parents);

void resolved_package_free(ResolvedPackage* ptr);
void resolved_package_reset_children(ResolvedPackage* ptr);

/* ResolvedPackage operations */
ResolvedEbuild* rp_current(ResolvedPackage* pkg);
ResolvedEbuild* rp_next(ResolvedPackage* pkg);
ResolvedEbuild* re_next(ResolvedEbuild* pkg);

#endif //CPORTAGE_RESOLVE_H
