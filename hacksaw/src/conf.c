//
// Created by atuser on 10/21/17.
//

#include <autogentoo/hacksaw/tools.h>
#include <string.h>
#include <stdlib.h>

Conf* conf_new(char* path) {
	Conf* conf = malloc(sizeof(Conf));
	
	conf->path = strdup(path);
	conf->sections = vector_new(sizeof(ConfSection*), REMOVE | UNORDERED);
	conf->default_variables = vector_new(sizeof(ConfVariable), REMOVE | UNORDERED);
	
	char* line;
	size_t len = 0;
	ssize_t read;
	FILE* fp = fopen(path, "r");
	
	if (fp == NULL) {
		lerror("no such file or directory: %s", path);
		exit(1);
	}
	
	ConfSection* current_section = NULL;
	Vector* section_locations = vector_new(sizeof(long), ORDERED | KEEP);
	char current_section_name[32];
	long ftell_last = ftell(fp);
	while ((read = getline(&line, &len, fp)) != -1) {
		line[strlen(line) - 1] = '\0'; // Remove the newline
		if (line[0] == '#')
			continue;
		if (regex_simple(current_section_name, line, "^\\[(.*?)\\]$") != 0) {
			vector_add(section_locations, &ftell_last);
			current_section = conf_section_new(current_section_name);
			vector_add(conf->sections, &current_section);
			current_section->parent = conf;
		}
		ftell_last = ftell(fp);
	}
	ftell_last = ftell(fp);
	vector_add(section_locations, &ftell_last);
	conf_section_read(conf, NULL,
					  conf->default_variables,
					  0,
					  *(long*) vector_get(section_locations, 0), fp);
	int i;
	for (i = 0; i < conf->sections->n; i++) {
		conf_section_read(conf, *(ConfSection**) vector_get(conf->sections, i),
						  (*(ConfSection**) vector_get(conf->sections, i))->variables,
						  *(long*) vector_get(section_locations, i),
						  *(long*) vector_get(section_locations, i + 1), fp);
	}
	
	fclose(fp);
	fp = NULL;
	return conf;
}

void conf_section_read(Conf* conf, ConfSection* section, Vector* variables, long start, long stop, FILE* fp) {
	fseek(fp, start, SEEK_SET);
	size_t size = (size_t) (stop - start);
	
	char* buffer = malloc(size + 1);
	fread(buffer, sizeof(char), size, fp);
	buffer[size] = 0;
	Vector* var_buff = regex_full(buffer,
								  "(\\S+)[\\s*]?=(?:(?=.*\\\")\\s*\\\"([^\\\"|]*)\\\"|(?=.*\\')\\s*\\'([^\\']*)\\'|(?!.*[\\\"|\\'])\\s*(.*))");
	int i;
	for (i = 0; i != var_buff->n; i++) {
		StringVector* current_vector = *(StringVector**) vector_get(var_buff, i);
		ConfVariable var;
		conf_variable_new(conf, section, &var, current_vector);
		vector_add(variables, &var);
		string_vector_free(current_vector);
	}
	vector_free(var_buff);
}

ConfSection* conf_section_new(char* name) {
	ConfSection* out = malloc(sizeof(ConfSection));
	out->variables = vector_new(sizeof(ConfVariable), REMOVE | UNORDERED);
	out->name, strdup(name);
	return out;
}

void conf_variable_new(Conf* conf, ConfSection* section, ConfVariable* var, StringVector* data) {
	var->identifier = strdup(string_vector_get(data, 0));
	char* value_buf = string_vector_get(data, 1);
	size_t len = strlen(value_buf);
	var->value = malloc(sizeof(char) * (len + (len % 16)));
	strcpy(var->value, value_buf);
	var->parent = conf;
	var->parent_section = section;
}

void conf_free(Conf* conf) {
	int i;
	for (i = 0; i != conf->default_variables->n; i++) {
		conf_variable_free((ConfVariable*)vector_get(conf->default_variables, i));
	}
	vector_free(conf->default_variables);
	for (i = 0; i != conf->sections->n; i++) {
		conf_section_free(*(ConfSection**) vector_get(conf->sections, i));
	}
	vector_free(conf->sections);
	
	free(conf);
}

void conf_section_free(ConfSection* section) {
	int i;
	for (i = 0; i != section->variables->n; i++) {
		conf_variable_free((ConfVariable*)vector_get(section->variables, i));
	}
	vector_free(section->variables);
}

void conf_variable_free(ConfVariable* var) {
	free(var->value);
}

char* conf_get(Conf* conf, char* section, char* variable_name) {
	if (section == NULL) {
		int j;
		for (j = 0; j != conf->default_variables->n; j++) {
			if (strcmp(variable_name, ((ConfVariable*) vector_get(conf->default_variables, j))->identifier) == 0) {
				return ((ConfVariable*) vector_get(conf->default_variables, j))->value;
			}
		}
		return NULL;
	}
	
	int i;
	for (i = 0; i != conf->sections->n; i++) {
		ConfSection* current_section = *(ConfSection**) vector_get(conf->sections, i);
		if (strcmp(section, current_section->name) == 0) {
			int j;
			for (j = 0; j != current_section->variables->n; j++) {
				if (strcmp(variable_name, ((ConfVariable*) vector_get(current_section->variables, j))->identifier) ==
					0) {
					return ((ConfVariable*) vector_get(current_section->variables, j))->value;
				}
			}
		}
	}
	return NULL;
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