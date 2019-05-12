//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H

typedef struct __Package Package;
typedef struct __Ebuild Ebuild;

#include <autogentoo/hacksaw/vector.h>
#include <stdint.h>
#include <autogentoo/hacksaw/string_vector.h>
#include "constants.h"
#include "atom.h"
#include "keywords.h"

struct __Package {
	uint32_t hash;
	
	char* key; // category/name
	char* category;
	char* name;
	
	Keyword* keywords;
	Ebuild* ebuilds;
};

#include "use.h"
#include "manifest.h"

/**
 * Version of a specfic package, this is what is built
 */
struct __Ebuild {
	char* category;
	char* pn;
	char* pv;
	char* pr;
	char* slot;
	char* eapi;
	
	/* Uninitalized until we emerge it */
	char* p;
	char* pvr;
	char* pf;
	
	/* Cached in the database */
	Dependency* depend;
	Dependency* bdepend;
	Dependency* rdepend;
	Dependency* pdepend; //!< Post install dependencies so no circular depends
	
	Vector* use; //!< Read iuse, then apply globals (make.conf), then package.use
	Vector* feature_restrict;
	keyword_t keywords[ARCH_END];
	
	RequiredUse* required_use;
	Dependency* src_uri;
	
	AtomVersion* version;
	int revision;
	
	sha_hash meta_sha512;
	
	Manifest* atom_manifest;
	Manifest* category_manifest;
	int metadata_init;
	
	Ebuild* older;
	Ebuild* newer;
};

struct __DependencyTree {
	DependencyTree* head;
	DependencyTree* tail;
	
	DependencyTree* next;
	DependencyTree* back;
	
	Ebuild* depend;
};

Dependency* dependency_build_atom(P_Atom* atom);
Dependency* dependency_build_use(char* use_flag, use_select_t type, Dependency* selector);

void package_metadata_init(Ebuild* ebuild);
Ebuild* package_init(Repository* repo, Manifest* category_man, Manifest* atom_man);

Ebuild* atom_resolve_ebuild(Repository* repo, P_Atom* atom, arch_t target);
DependencyTree* package_resolve_dependencies(Emerge* emerge, char* atom);

#endif //AUTOGENTOO_PACKAGE_H
