//
// Created by atuser on 5/9/19.
//

#ifndef AUTOGENTOO_EMERGE_H
#define AUTOGENTOO_EMERGE_H

#include <autogentoo/hacksaw/map.h>
#include "atom.h"
#include "constants.h"
#include "cportage_defines.h"
#include "profile.h"

typedef enum {
	EMERGE_NO_OPTS,
	EMERGE_QUIET = 1 << 0,
	EMERGE_ASK = 1 << 1,
	EMERGE_DEEP = 1 << 2,
	EMERGE_IGNORE_DEFAULT = 1 << 3,
	EMERGE_BUILD_PKG = 1 << 4,
	EMERGE_USE_BINHOST = 1 << 5,
	EMERGE_ONESHOT = 1 << 6,
	EMERGE_BUILDROOT = 1 << 7,
	EMERGE_INSTALLROOT = 1 << 8,
	EMERGE_VERBOSE = 1 << 9,
} emerge_opts_t;

typedef enum {
	EMERGE_RUNSIDE,
	EMERGE_BUILDSIDE
} emerge_build_t;

struct __Emerge {
	Repository* repos;
	
	char* default_repo_name;
	Repository* default_repo;
	
	char** atoms; //!< Install arguments
	char* root;
	arch_t target_arch;
	emerge_opts_t options;
	emerge_build_t build_opts;
	
	char* buildroot;
	char* installroot;
	
	sha_hash hash_accept_keywords;
	sha_hash hash_package_use;
	sha_hash hash_make_conf;
	
	PortageDB* database;
	Vector* selected;
	
	Map* use_expand;
	Profile* profile;
	
	Suggestion* use_suggestions;
	Suggestion* keyword_suggestions;
};

extern Emerge* emerge_main;

Emerge* emerge_new();
int emerge (Emerge* emg);
void emerge_free(Emerge* em);

#endif //AUTOGENTOO_EMERGE_H
