//
// Created by atuser on 10/12/19.
//

#ifndef AUTOGENTOO_PROFILE_H
#define AUTOGENTOO_PROFILE_H

#include <autogentoo/hacksaw/vector.h>
#include "constants.h"

typedef struct __Profile Profile;

struct __Profile {
	Vector* system_set;
	
	Vector* use_mask;
	Vector* package_mask;
	
	UseFlag* globals;
	
	Vector* use_force;
	Vector* package_use_force; // precedence over use_force
};

void profile_parse(Profile* update, char* current_path, char* path);
void profile_free(Profile* ptr);

#endif //AUTOGENTOO_PROFILE_H
