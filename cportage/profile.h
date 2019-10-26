//
// Created by atuser on 10/12/19.
//

#ifndef AUTOGENTOO_PROFILE_H
#define AUTOGENTOO_PROFILE_H

typedef struct __Profile Profile;
typedef struct __PackageMask PackageMask;

#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/map.h>
#include "atom.h"

typedef enum {
	PACKAGE_MASK,
	PACKAGE_UNMASK
} mask_t;

struct __PackageMask {
	P_Atom* atom;
	mask_t mask;
	
	PackageMask* next;
	PackageMask** last;
};

/**
 * Directly from 'man portage'
 *  /etc/portage/make.profile/ or /etc/make.profile/
              site-specific overrides go in /etc/portage/profile/
      deprecated
      eapi
      make.defaults
      packages
      packages.build
      package.accept_keywords
      package.bashrc
      package.keywords
      package.mask
      package.provided
      package.unmask
      package.use
      package.use.force
      package.use.mask
      package.use.stable.force
      package.use.stable.mask
      parent
      profile.bashrc
      soname.provided
      use.force
      use.mask
      use.stable.mask
      use.stable.force
      virtuals
 */
struct __Profile {
	/* make.defaults also passed into make.conf */
	Map* make_conf;
	
	/* The system set */
	Vector* packages;
	
	/* The system set */
	Vector* profile_packages;
	
	/* Used for stage1 and stage2, We can do this now yay!! */
	Vector* package_build;
	
	/* package.keywords overrided by non-deprecated package.accept_keywords */
	/* above keywords overrided by /etc/portage/package.accept_keywords */
	Vector* package_accept_keywords;
	
	/* Sourced after profile.bashrc, inside the bashrc directory in profile */
	Vector* package_bashrc;
	
	/* Package mask */
	/* Mark an ebuild as masked */
	Vector* package_mask;
	
	/* Create a fake install folder in db */
	Vector* package_provided;
	
	/*  */
	/* overrided by /etc/portage/package.use */
	Vector* package_use;
	
	Map* use;
	
	/* Only use for use defaults */
	Map* implicit_use;
};

Profile* profile_new();
void profile_implicit_use(Profile* update);
void profile_parse(Profile* update, char* current_path, char* path);
void profile_free(Profile* ptr);

#endif //AUTOGENTOO_PROFILE_H
