//
// Created by atuser on 11/22/17.
//

#ifndef HACKSAW_PORTAGE_H
#define HACKSAW_PORTAGE_H

#include "../tools.h"
#include "../language.h"
#include "repository.h"

typedef struct __Portage Portage;

struct __Portage {
	char* root_dir;
	RepoConfig* repos;

};

#endif
