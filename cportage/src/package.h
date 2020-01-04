//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H

#include <autogentoo/hacksaw/vector.h>
#include <stdint.h>
#include <autogentoo/hacksaw/string_vector.h>
#include "constants.h"
#include "atom.h"
#include "keywords.h"

struct __Package {
	Repository* parent;
	uint32_t hash;
	
	char* key; // category/name
	char* category;
	char* name;
	
	Keyword* keywords;
	PackageUse* useflags;
	Ebuild* ebuilds;
};

#include "use.h"
#include "ebuild/manifest.h"
#include "portage_log.h"

/**
 * Version of a specfic package, this is what is built
 */
struct __Ebuild {
	Package* parent;
	char* name;
	char* category;
	
	char* slot;
	char* sub_slot;
	
	char* pf;
	
	char* ebuild_key;
	char* ebuild_md5;
	char* ebuild;
	char* path;
	char* cache_file;
	
	/* Cached in the database */
	Dependency* depend;
	Dependency* bdepend;
	Dependency* rdepend;
	Dependency* pdepend; //!< Must be installed after this package is
	
	UseFlag* use; //!< Read iuse, then apply globals (make.conf), then package.use
	Vector* feature_restrict;
	keyword_t keywords[ARCH_END];
	
	RequiredUse* required_use;
	Dependency* src_uri;
	
	AtomVersion* version;
	int revision;
	
	sha_hash meta_sha512;
	int metadata_init;
	
	Ebuild* older;
	Ebuild* newer;
};

Dependency* dependency_build_atom(P_Atom* atom);
Dependency* dependency_build_use(char* use_flag, use_t type, Dependency* selector);

void package_metadata_init(Ebuild* ebuild);
Ebuild* package_init(Repository* repo, char* category, char* name);

void package_free(Package* ptr);
void ebuild_free(Ebuild* ptr);

#endif //AUTOGENTOO_PACKAGE_H
