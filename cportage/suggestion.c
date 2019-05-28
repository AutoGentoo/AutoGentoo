//
// Created by atuser on 5/26/19.
//

#define _GNU_SOURCE

#include "suggestion.h"
#include "keywords.h"
#include "package.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void emerge_suggestion_keyword(Emerge* emerge, Ebuild* required_by, P_Atom* selector) {
	Suggestion* new_sugg = malloc(sizeof(Suggestion));
	
	char* arch_str = NULL;
	
	for (int i = 0; i < ARCH_END; i++) {
		if (emerge->target_arch == keyword_links[i].l)
			arch_str = strdup(keyword_links[i].str);
	}
	
	char* atom = atom_get_str(selector);
	asprintf(&new_sugg->line_addition, "%s ~%s", atom, arch_str);
	
	new_sugg->portage_file = strdup("package.keywords");
	asprintf(&new_sugg->required_by, "# Required by %s-%s", required_by->parent->key, required_by->version->full_version);
	
	new_sugg->next = emerge->keyword_suggestions;
	emerge->keyword_suggestions = new_sugg;
	
	free(atom);
	free(arch_str);
}
