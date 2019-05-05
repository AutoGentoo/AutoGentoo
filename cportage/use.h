//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_REQUIRE_USE_H
#define AUTOGENTOO_REQUIRE_USE_H


typedef struct __RequiredUse RequiredUse;
typedef struct __IUSE IUSE;

#include "package.h"

struct __RequiredUse {
	char* target;
	use_select_t option;
	RequiredUse* depend;
	RequiredUse* next;
};

RequiredUse* use_build_required_use(char* target, use_select_t option);

#endif //AUTOGENTOO_REQUIRE_USE_H
