//
// Created by atuser on 5/9/19.
//

#ifndef AUTOGENTOO_EMERGE_H
#define AUTOGENTOO_EMERGE_H

typedef struct __Emerge Emerge;

#include "atom.h"
#include "portage.h"

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
} emerge_opts_t;

struct __Emerge {
	char* default_repo;
	Repository* repo;
	char** atoms; //!< Install arguments
	char* root;
	arch_t target_arch;
	emerge_opts_t options;
	
	char* buildroot;
	char* installroot;
	
	sha_hash hash_accept_keywords;
	sha_hash hash_package_use;
	sha_hash hash_make_conf;
};

Emerge* emerge_new();
int emerge (Emerge* emg);

#endif //AUTOGENTOO_EMERGE_H
