//
// Created by atuser on 12/29/19.
//

#ifndef AUTOGENTOO_RESOLVED_EBUILD_H
#define AUTOGENTOO_RESOLVED_EBUILD_H

typedef struct __ResolvedEbuild ResolvedEbuild;

#include "../constants.h"
#include "../use.h"


typedef enum {
	PORTAGE_NEW = 1 << 0, //!< New package
	PORTAGE_SLOT = 1 << 1, //!< Install side by side
	PORTAGE_REMOVE = 1 << 2, //!< Remove package 'old'
	PORTAGE_UPDATE = 1 << 3, //!< Install a new package and remove the old one
	PORTAGE_DOWNGRADE = 1 << 4,
	PORTAGE_REPLACE = 1 << 5,
	PORTAGE_USE_FLAG = 1 << 6,
	PORTAGE_BLOCK = 1 << 7
} action_t;

/**
 * This struct will hold data about a selected ebuild
 *
 * This is what is used to build an ebuild during
 * the ebuild phase.
 */
struct __ResolvedEbuild {
	Emerge* environ;
	P_Atom* atom;
	
	Ebuild* target;
	InstalledEbuild* installed;
	
	UseFlag* use;
	UseFlag* explicit;
	
	action_t action;
};

ResolvedEbuild* re_new(Emerge* environ, P_Atom* atom, Ebuild* ebuild);

#endif //AUTOGENTOO_RESOLVED_EBUILD_H
