//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_REQUIRE_USE_H
#define AUTOGENTOO_REQUIRE_USE_H

#include "constants.h"
#include "atom.h"

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

use_select_t package_check_use(Ebuild* ebuild, char* useflag);
RequiredUse* use_build_required_use(char* target, use_select_t option);

#endif //AUTOGENTOO_REQUIRE_USE_H
