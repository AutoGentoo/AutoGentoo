#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/portage/use_flags.h>

IUSE read_iuse(Ebuild* ebuild) {
	char* temp_use = conf_get(ebuild->metadata, "", "IUSE");
	IUSE out = iuse_new();
	iuse_parse(out, temp_use);
	
	return out;
}

IUSE iuse_new() {
	return map_new(sizeof(Use), 16);
}

void iuse_parse(IUSE to_update, char* iuse_str) {
	StringVector* split = string_vector_new();
	string_vector_split(split, iuse_str, " ");
	
	int i;
	for (i = 0; i != split->n; i++) {
		char* temp = string_vector_get(split, i);
		use_t status = NO_USE;
		if (temp[0] == '+') {
			status = YES_USE;
			temp = &temp[1];
		}
		if (temp[0] == '-') {
			status = NO_USE;
			temp = &temp[1];
		}
		
		Use* use_temp = malloc(sizeof(Use));
		use_temp->str = strdup(temp);
		map_insert(to_update, temp, use_temp);
	}
	
	string_vector_free(split);
}