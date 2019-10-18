//
// Created by atuser on 10/11/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include "globals.h"
#include "directory.h"
#include "portage.h"
#include "emerge.h"
#include <string.h>
#include <ctype.h>
#include <share.h>

char* strupr(char* str) {
	for (char* c = str; *c; c++)
		*c = (char)toupper(*c);
	return str;
}

char* strlwr(char* str);

Map* use_expand_new(Repository* repo) {
	char* use_expand_location = NULL;
	asprintf(&use_expand_location, "%s/profiles/desc", repo->location);
	
	FPNode* use_expand_dir = open_directory(use_expand_location);
	free(use_expand_location);
	
	Map* out = map_new(128, 0.8);
	
	for (FPNode* dir = use_expand_dir; dir; dir = dir->next) {
		char* dot_split = strchr(dir->filename, '.');
		char* key_name = strupr(strndup(dir->filename, dot_split - dir->filename));
		
		map_insert(out, key_name, NULL); // Just create a set
		free(key_name);
	}
	
	fpnode_free(use_expand_dir);
	
	return out;
}


Map* make_conf_new(Emerge *em) {
	char* filename = NULL;
	asprintf(&filename, "%s/etc/portage/make.conf", em->root);
	
	FILE* fp = fopen(filename, "r");
	Map* out = map_new(256, 0.8);
	mc_parse(fp, out);
	
	fclose(fp);
	
	return out;
}

Map * make_conf_use(Emerge* em) {
	StringVector* keys = map_all_keys(em->use_expand);
	Map* out = map_new(256, 0.8);
	
	
	/* USE_EXPAND */
	for (int i = 0; i < keys->n; i++) {
		char* key = string_vector_get(keys, i);
		char* value = map_get(em->make_conf, key);
		
		if (!value)
			continue;
		
		key = strlwr(strdup(key));
		value = strdup(value);
		
		for (char* tok = strtok(value, " "); tok; tok = strtok(NULL, " ")) {
			use_select_t* status = malloc(sizeof(use_select_t));
			char* name = NULL;
			*status = USE_ENABLE;
			
			if (tok[0] == '-') {
				*status = USE_DISABLE;
				asprintf(&name, "%s_%s", key, tok + 1);
			}
			else
				asprintf(&name, "%s_%s", key, tok);
			free(map_insert(out, name, status));
			free(name);
		}
		free(value);
		free(key);
	}
	
	char* use_str = map_get(em->make_conf, "USE");
	if (use_str) {
		use_str = strdup(use_str);
		for (char* tok = strtok(use_str, " "); tok; tok = strtok(NULL, " ")) {
			use_select_t* status = malloc(sizeof(use_select_t));
			*status = USE_ENABLE;
			
			if (tok[0] == '-') {
				*status = USE_DISABLE;
				tok++;
			}
			
			free(map_insert(out, tok, status));
		}
		free(use_str);
	}
	
	return out;
}

char* prv_str_replace(char *orig, char *rep, char *with) {
	char* result; // the return string
	char* ins;    // the next insert point
	char* tmp;    // varies
	size_t len_rep;  // length of rep (the string to remove)
	size_t len_with; // length of with (the string to replace rep with)
	size_t len_front; // distance between rep and end of last rep
	int count;    // number of replacements
	
	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	if (!with)
		with = "";
	len_with = strlen(with);
	
	// count the number of replacements needed
	ins = orig;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}
	
	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
	
	if (!result)
		return NULL;
	
	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

void make_conf_add(Map* make_conf, char* key, char* value) {
	char* old_value = map_get(make_conf, key);
	
	if (!old_value) {
		map_insert(make_conf, key, value);
		return;
	}
	
	char* replace_key = NULL;
	asprintf(&replace_key, "${%s}", key);
	char* new_value = prv_str_replace(value, replace_key, old_value);
	
	printf("%s = %s\n", key, new_value);
	
	free(replace_key);
	
	free(map_insert(make_conf, key, new_value));
}