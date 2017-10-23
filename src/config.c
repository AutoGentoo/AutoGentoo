//
// Created by atuser on 10/21/17.
//

#include <tools/config.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <stdlib.h>
#include <tools/string.h>
#include <test/debug.h>

Config* config_read (char* config_path) {
    Config* new_config = malloc (sizeof(Config));

    strcpy(new_config->path, config_path);
    new_config->sections = vector_new(sizeof(ConfigSection*), REMOVE | UNORDERED);
    new_config->default_variables = vector_new(sizeof(ConfigVariable), REMOVE | UNORDERED);

    config_add(new_config, new_config->path);
    return new_config;
}

void config_add (Config* config, char* path) {
    char* line;
    size_t len = 0;
    ssize_t read;
    FILE* fp = fopen(path, "r");

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
        }
        ftell_last = ftell(fp);
    }
    ftell_last = ftell(fp);
    vector_add(section_locations, &ftell_last);
    config_section_read(fp, config->default_variables,
                        0,
                        *(long*)vector_get(section_locations, 0));
    int i;
    for (i = 0; i < config->sections->n; i++) {
        config_section_read(fp, (*(ConfigSection**)vector_get(config->sections, i))->variables,
                            *(long*)vector_get(section_locations, i),
                            *(long*)vector_get(section_locations, i + 1));
    }

    fclose(fp);
    fp = NULL;
}

void config_section_read (FILE* fp, Vector* variables, long start, long stop) {
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
        config_variable_new(&var, current_vector);
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

void config_variable_new (ConfigVariable* var, StringVector* data) {
    strcpy (var->identifier, string_vector_get(data, 0));
    char* value_buf = string_vector_get(data, 1);
    size_t len = strlen(value_buf);
    var->value = malloc (sizeof(char) * (len + (len % 16)));
    strcpy(var->value, value_buf);
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
            if (strcmp(variable_name, vector_get(config->default_variables, j)) == 0) {
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
                if (strcmp(variable_name, vector_get(current_section->variables, j)) == 0) {
                    return ((ConfigVariable*)vector_get(current_section->variables, j))->value;
                }
            }
        }
    }
    return NULL;
}