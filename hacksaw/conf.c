//
// Created by atuser on 10/21/17.
//

#define _GNU_SOURCE
#include "hacksaw/hacksaw.h"
#include <string.h>
#include <stdlib.h>

Conf* conf_new(char* path) {
	Conf* conf = malloc(sizeof(Conf));
	
	conf->path = strdup(path);
	conf->variables = map_new(32, 0.85);
	conf->sections = string_vector_new();

	char* file;
	size_t len = 0;
	ssize_t read;
	char* line;
	FILE* fp = fopen(path, "r");
	
	if (fp == NULL) {
		lerror("no such file or directory: %s", path);
		exit(1);
	}
	
	//char* regex_match = "^\\[(?<section>\\w+)\\]\\s*?$\n"
	//			"|^(?<field>[\\w-]+)[\\s]*=[\\s]*(?:\"(?<quotedstr>(?:\\\\.|[^\\\\\"]++)*+)\"|(?<barestr>.*?))\\s*?$";

	char* current_section = "";
	while ((read = getline(&line, &len, fp)) != -1) {
		size_t length = strlen(line) - 1;
		line[length] = '\0'; // Remove the newline
		if (!length || line[0] == '#')
			continue;
		
		if (line[0] == '[' && line[length - 1] == ']') {
			if (current_section[0])
				free(current_section);
			current_section = strdup(line);
			char* striped_section = strndup (current_section + 1, strlen(current_section) - 2);
			string_vector_add(conf->sections, striped_section);
			continue;
		}
		
		char* content_split = strchr(line, ' ');
		*content_split = 0;
		
		char* new_key;
		asprintf(&new_key, "%s(%s)", current_section, line);
		
		char* value = string_strip(strchr(content_split + 1, '=') + 1);
		printf ("%s = '%s'\n", new_key, value);
		map_insert(conf->variables, new_key, value);
	}

	fclose(fp);
	fp = NULL;
	return conf;
}

void conf_free(Conf* conf) {
	map_free(conf->variables, free);
	free(conf);
}

char* conf_get(Conf* conf, char* section, char* variable_name) {
	if (section == NULL)
		section = "";
	
	char* key;
	asprintf(&key, "[%s](%s)", section, variable_name);
	
	printf("get(%s)\n", key);
	
	char* out = map_get(conf->variables, key);
	free (key);
	return out;
}

int conf_get_convert(Conf* conf, char* dest, char* section, char* variable_name) {
	char* buffer = conf_get(conf, section, variable_name);
	if (buffer != NULL) {
		strcpy(dest, buffer);
		return 1;
	}
	return 0;
}

StringVector* conf_get_vector(Conf* conf, char* section, char* variable_name) {
	StringVector* out = string_vector_new();
	string_vector_split(out, conf_get(conf, section, variable_name), " ");
	return out;
}