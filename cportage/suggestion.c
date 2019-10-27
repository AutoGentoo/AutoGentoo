//
// Created by atuser on 5/26/19.
//

#define _GNU_SOURCE

#include "suggestion.h"
#include "cportage_defines.h"
#include "emerge.h"
#include "use.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


Suggestion* suggestion_new(char* required_by, char* line, ...) {
	Suggestion* out = malloc(sizeof(Suggestion));
	
	out->next = NULL;
	out->line_addition = NULL;
	out->required_by = strdup(required_by);
	
	va_list args;
	va_start (args, line);
	vasprintf(&out->line_addition, line, args);
	va_end(args);
	
	return out;
}

FILE* suggestion_read(Suggestion* s) {
	FILE* out = tmpfile();
	
	for (Suggestion* c = s; c; c = c->next) {
		fwrite(c->line_addition, strlen(c->line_addition), 1, out);
		fwrite("\n", 1, 1, out);
	}
	
	fseek(out, 0, SEEK_SET);
	return out;
}

void emerge_apply_suggestions(Emerge* em) {
	FILE* to_apply = suggestion_read(em->use_suggestions);
	useflag_parse(to_apply, em->profile->package_use, KEYWORD_UNSTABLE, PRIORITY_NORMAL);
	fclose(to_apply);
}