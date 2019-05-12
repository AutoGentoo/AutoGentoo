//
// Created by atuser on 5/5/19.
//

#include <stdio.h>
#include <string.h>
#include "use.h"
#include "package.h"
#include <stdlib.h>

use_select_t package_check_use(Ebuild* ebuild, char* useflag) {
	UseFlag* current;
	for (current = ebuild->use; current; current = current->next) {
		if (strcmp(current->name, useflag) == 0)
			return current->status;
	}
	
	return -1;
}

RequiredUse* use_build_required_use(char* target, use_select_t option) {
	RequiredUse* out = malloc(sizeof(RequiredUse));
	
	if (target)
		out->target = strdup(target);
	else
		out->target = NULL;
	out->option = option;
	out->depend = NULL;
	out->next = NULL;
	
	return out;
}