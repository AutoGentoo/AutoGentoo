//
// Created by atuser on 5/9/19.
//

#include "emerge.h"
#include <string.h>

Emerge* emerge_new() {
	Emerge* out = malloc(sizeof(Emerge));
	
	out->target_arch = ARCH_AMD64;
	
	out->atom = NULL;
	out->resolved_atom = NULL;
	out->buildroot = strdup("/");
	out->installroot = strdup("/");
	out->root = strdup("/");
	out->options = 0;
	out->repo = NULL;
	
	return out;
}

int emerge (Emerge* emg) {
	return 0;
}
