//
// Created by atuser on 5/5/19.
//

#include <stdio.h>
#include <string.h>
#include "use.h"

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