//
// Created by atuser on 10/21/17.
//

#include <config/config.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <stdlib.h>
#include <tools/log.h>

Config* config_new (char* path) {
    Config* config = malloc (sizeof(Config));

    strcpy(config->path, path);
    config->sections = vector_new(sizeof(ConfigSection*), REMOVE | UNORDERED);
    config->default_variables = vector_new(sizeof(ConfigVariable), REMOVE | UNORDERED);

    char* line;
    size_t len = 0;
    ssize_t read;
    FILE* fp = fopen(path, "r");

    if (fp == NULL) {
        lerror("no such file or directory: %s", path);
        exit(1);
    }

    ConfigSection* current_section = NULL;
    Vector* section_locations = vector_new(sizeof(long), ORDERED | KEEP);
    char current_section_name[32];
    long ftell_last = ftell(fp);
    while ((read = getline(&line, &len, fp)) != -1) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        if (line[0] == '#')
            continue;
        if (regex_simple(current_section_name, line, "^\\[(.*?)\\]$") != 0) {
            vector_add(section_locations, &ftell_last);
            current_section = config_section_new(current_section_name);
            vector_add(config->sections, &current_section);
            current_section->parent = config;
        }
        ftell_last = ftell(fp);
    }
    ftell_last = ftell(fp);
    vector_add(section_locations, &ftell_last);
    config_section_read(config, NULL,
                        config->default_variables,
                        0,
                        *(long*)vector_get(section_locations, 0), fp);
    int i;
    for (i = 0; i < config->sections->n; i++) {
        config_section_read(config, *(ConfigSection**)vector_get(config->sections, i),
                            (*(ConfigSection**)vector_get(config->sections, i))->variables,
                            *(long*)vector_get(section_locations, i),
                            *(long*)vector_get(section_locations, i + 1), fp);
    }

    fclose(fp);
    fp = NULL;
    return config;
}

void config_section_read (Config* config, ConfigSection* section, Vector* variables, long start, long stop, FILE* fp) {
    fseek(fp, start, SEEK_SET);
    size_t size = (size_t) (stop - start);

    char* buffer = malloc (size+1);
    fread (buffer, sizeof(char), size, fp);
    buffer[size] = 0;
    Vector* var_buff = regex_full(buffer, "(\\S+)[\\s*]?=(?:(?=.*\\\")\\s*\\\"([^\\\"|]*)\\\"|(?=.*\\')\\s*\\'([^\\']*)\\'|(?!.*[\\\"|\\'])\\s*(.*))");
    int i;
    for (i=0; i!=var_buff->n; i++) {
        StringVector* current_vector = *(StringVector**)vector_get(var_buff, i);
        ConfigVariable var;
        config_variable_new(config, section, &var, current_vector);
        vector_add(variables, &var);
        string_vector_free(current_vector);
    }
    vector_free(var_buff);
}

ConfigSection* config_section_new (char* name) {
    ConfigSection* out = malloc (sizeof (ConfigSection));
    out->variables = vector_new(sizeof(ConfigVariable), REMOVE | UNORDERED);
    strcpy(out->name, name);
    return out;
}

void config_variable_new (Config* config, ConfigSection* section, ConfigVariable* var, StringVector* data) {
    strcpy (var->identifier, string_vector_get(data, 0));
    char* value_buf = string_vector_get(data, 1);
    size_t len = strlen(value_buf);
    var->value = malloc (sizeof(char) * (len + (len % 16)));
    strcpy(var->value, value_buf);
    var->parent = config;
    var->parent_section = section;
}

void config_free (Config* config) {
    int i;
    for (i=0; i!=config->default_variables->n; i++) {
        config_variable_free(vector_get(config->default_variables, i));
    }
    vector_free(config->default_variables);
    for (i=0; i!=config->sections->n; i++) {
        config_section_free(*(ConfigSection**)vector_get(config->sections, i));
    }
    vector_free(config->sections);

    free(config);
}

void config_section_free (ConfigSection* section) {
    int i;
    for (i=0; i!=section->variables->n; i++) {
        config_variable_free(vector_get(section->variables, i));
    }
    vector_free(section->variables);
}

void config_variable_free (ConfigVariable* var) {
    free(var->value);
}

char* config_get (Config* config, char* section, char* variable_name) {
    if (section == NULL) {
        int j;
        for (j=0; j!=config->default_variables->n; j++) {
            if (strcmp(variable_name, ((ConfigVariable*)vector_get(config->default_variables, j))->identifier) == 0) {
                return ((ConfigVariable*)vector_get(config->default_variables, j))->value;
            }
        }
        return NULL;
    }

    int i;
    for (i=0; i!=config->sections->n; i++) {
        ConfigSection* current_section = *(ConfigSection**)vector_get(config->sections, i);
        if (strcmp(section, current_section->name) == 0) {
            int j;
            for (j=0; j!=current_section->variables->n; j++) {
                if (strcmp(variable_name, ((ConfigVariable*)vector_get(current_section->variables, j))->identifier) == 0) {
                    return ((ConfigVariable*)vector_get(current_section->variables, j))->value;
                }
            }
        }
    }
    return NULL;
}

int config_get_convert (Config* config, char* dest, char* section, char* variable_name) {
    char* buffer = config_get(config, section, variable_name);
    if (buffer != NULL) {
        strcpy(dest, buffer);
        return 1;
    }
    return 0;
}

StringVector* config_get_vector (Config* config, char* section, char* variable_name) {
    StringVector* out = string_vector_new();
    string_vector_split(out, config_get(config, section, variable_name), " ");
    return out;
}