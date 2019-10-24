//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_REQUIRE_USE_H
#define AUTOGENTOO_REQUIRE_USE_H

#include <autogentoo/hacksaw/vector.h>
#include "constants.h"
#include "atom.h"
#include "cportage_defines.h"

struct __PackageUse {
	P_Atom* atom;
	UseFlag* flags;
	keyword_t keyword_required; // At least this keyword must be present to apply (package.use.stable)
	PackageUse* next;
};

struct __UseFlag {
	char* name;
	use_select_t status; // Only USE_DISABLE and USE_ENABLE
	use_priority_t priority;
	UseFlag* next;
};

struct __RequiredUse {
	char* target;
	use_select_t option;
	RequiredUse* depend;
	RequiredUse* next;
};

use_select_t ebuild_set_use(Ebuild* ebuild, char* useflag, use_select_t new_val);
use_select_t s_ebuild_set_use(SelectedEbuild* ebuild, char* useflag, use_select_t new_val, use_priority_t priority);
UseFlag* get_use(UseFlag* useflags, char* useflag);
RequiredUse* use_build_required_use(char* target, use_select_t option);
int ebuild_check_required_use(SelectedEbuild *ebuild);
UseFlag* useflag_new(char* name, use_select_t status, use_priority_t priority);
UseFlag * useflag_iuse_parse(Emerge* em, char *metadata);
void useflag_free(UseFlag* ptr);
void requireduse_free(RequiredUse* ptr);
AtomFlag* dependency_useflag(Ebuild* resolved, AtomFlag* new_flags, AtomFlag* old_flags);

void useflag_parse(FILE* fp, Vector* useflags, keyword_t keyword_required, use_priority_t priority);
void emerge_parse_useflags(Emerge* emerge);

#endif //AUTOGENTOO_REQUIRE_USE_H
