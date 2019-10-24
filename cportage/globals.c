//
// Created by atuser on 10/11/19.
//

#define _GNU_SOURCE

#include <stdio.h>
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


void make_conf_parse(Emerge *em) {
	char* filename = NULL;
	asprintf(&filename, "%s/etc/portage/make.conf", em->root);
	
	FILE* fp = fopen(filename, "r");
	mc_parse(fp, em->profile->make_conf);
	fclose(fp);
}

void make_conf_use(Emerge* em) {
	StringVector* keys = map_all_keys(em->use_expand);
	
	/* USE_EXPAND */
	for (int i = 0; i < keys->n; i++) {
		char* key = string_vector_get(keys, i);
		char* value = map_get(em->profile->make_conf, key);
		
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
			free(map_insert(em->profile->use, name, status));
			free(name);
		}
		free(value);
		free(key);
	}
	
	char* use_str = map_get(em->profile->make_conf, "USE");
	if (use_str) {
		use_str = strdup(use_str);
		for (char* tok = strtok(use_str, " "); tok; tok = strtok(NULL, " ")) {
			use_select_t* status = malloc(sizeof(use_select_t));
			*status = USE_ENABLE;
			
			if (tok[0] == '-') {
				*status = USE_DISABLE;
				tok++;
			}
			
			free(map_insert(em->profile->use, tok, status));
		}
		free(use_str);
	}
}

char* prv_str_replace(char* s, char* oldW, char* newW) {
	char *result;
	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);
	
	// Counting the number of times old word
	// occur in the string
	for (i = 0; s[i] != '\0'; i++)
	{
		if (strstr(&s[i], oldW) == &s[i])
		{
			cnt++;
			
			// Jumping to index after the old word.
			i += oldWlen - 1;
		}
	}
	
	// Making new string of enough length
	result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
	
	i = 0;
	while (*s)
	{
		// compare the substring with the result
		if (strstr(s, oldW) == s)
		{
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}
	
	result[i] = '\0';
	return result;
}

void make_conf_add(Map* make_conf, char* key, char* value, int is_profile) {
	int is_incremental = 0;
	if (strcmp(key, "USE") == 0 || strcmp(key, "ACCEPT_KEYWORDS") == 0 || strncmp(key, "CONFIG_PROTECT", 14) == 0)
		is_incremental = 1;
	
	char* old_value = map_get(make_conf, key);
	
	char* replace_key = NULL;
	char* new_value = NULL;
	asprintf(&replace_key, "${%s}", key);
	
	if (!old_value) {
		new_value = prv_str_replace(value, replace_key, "");
		map_insert(make_conf, key, new_value);
		free(value);
		free(replace_key);
		return;
	}
	
	if (is_incremental) {
		char* incremental_buffer = NULL;
		asprintf(&incremental_buffer, "%s %s", old_value, value);
		new_value = prv_str_replace(incremental_buffer, replace_key, "");
		free(incremental_buffer);
	}
	else
		new_value = prv_str_replace(value, replace_key, "");
	
	free(replace_key);
	free(value);
	free(map_insert(make_conf, key, new_value));
}