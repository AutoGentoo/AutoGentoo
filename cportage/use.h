//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_REQUIRE_USE_H
#define AUTOGENTOO_REQUIRE_USE_H

#include "constants.h"
#include "atom.h"

struct __PackageUse {
	P_Atom* atom;
	UseFlag* flags;
	PackageUse* next;
};

struct __UseFlag {
	char* name;
	use_select_t status; // Only USE_DISABLE and USE_ENABLE
	UseFlag* next;
};

struct __RequiredUse {
	char* target;
	use_select_t option;
	RequiredUse* depend;
	RequiredUse* next;
};

use_select_t ebuild_set_use(Ebuild* ebuild, char* useflag, use_select_t new_val);
use_select_t s_ebuild_set_use(SelectedEbuild* ebuild, char* useflag, use_select_t new_val);
UseFlag* s_ebuild_get_use(SelectedEbuild *ebuild, char* useflag);
use_select_t ebuild_check_use(Ebuild *ebuild, char* useflag);
RequiredUse* use_build_required_use(char* target, use_select_t option);
int ebuild_check_required_use(SelectedEbuild *ebuild);
UseFlag* useflag_new(char* name, use_select_t status);
void useflag_free(UseFlag* ptr);
void requireduse_free(RequiredUse* ptr);
AtomFlag* dependency_useflag(Ebuild* resolved, AtomFlag* new_flags, AtomFlag* old_flags);
void emerge_parse_useflags(Emerge* emerge);

#endif //AUTOGENTOO_REQUIRE_USE_H
