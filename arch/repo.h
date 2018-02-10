//
// Created by atuser on 1/5/18.
//

#ifndef AUTOGENTOO_ABS_REPO_H
#define AUTOGENTOO_ABS_REPO_H

#include "aabs.h"

typedef struct __aabs_repo_t aabs_repo_t;

struct __aabs_repo_t {
	char* name;
	aabs_svec_t* mirrors;
};


#endif //AUTOGENTOO_ABS_REPO_H
