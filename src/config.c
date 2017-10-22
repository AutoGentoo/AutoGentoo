//
// Created by atuser on 10/21/17.
//

#include <tools/config.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <portage/repository.h>
#include <stdlib.h>

Config* config_read (char* config_path) {
    Config* new_config = malloc (sizeof(Config));
    char* line;
    size_t len = 0;
    ssize_t read;

    strcpy(new_config->path, config_path);
    new_config->fp = fopen(new_config->path, "r");
    new_config->sections = vector_new(sizeof(ConfigSection), REMOVE | UNORDERED);
    new_config->default_variables = vector_new(sizeof(ConfigVariable), REMOVE | UNORDERED);

    ConfigSection* current_section = NULL;
    char current_section_name[32];

    while ((read = getline(&line, &len, new_config->fp)) != -1) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        if (line[0] == '#')
            continue;

        if (re_group_get(current_section_name, line, "\\[(.*?)\\]") != 0) {
            vector_add(new_config->sections, config_section_new(current_section_name));
            current_section = (ConfigSection*)vector_get(new_config->sections, new_config->sections->n-1);
        }

        StringVector* variable_vector = string_vector_new();
        int got;
        if ((got = re_group_match(variable_vector, line, "(.*) = (.*)", 2)) != 2) {
            string_vector_free(variable_vector);
            continue;
        }
        ConfigVariable b_var;
        config_variable_new (&b_var, variable_vector);

        if (current_section == NULL) {
            vector_add(new_config->default_variables, &b_var);
        }
        else {
            vector_add(current_section->variables, &b_var);
        }
        string_vector_free(variable_vector);
    }
    fclose(new_config->fp);
    new_config->fp = NULL;
    return new_config;
}

ConfigSection* config_section_new (char* name) {
    ConfigSection* out = malloc (sizeof (ConfigSection));
    strcpy(out->name, name);
    out->variables = vector_new(sizeof(ConfigVariable), UNORDERED | REMOVE);
    return out;
}

void config_variable_new (ConfigVariable* var, StringVector* data) {
    strcpy (var->identifier, string_vector_get(data, 0));
    char* value_buf = string_vector_get(data, 1);
    var->value = malloc (sizeof(char) * (strlen(value_buf) + 32));
    strcpy(var->value, value_buf);
}

void config_free (Config* config) {
    int i;
    for (i=0; i!=config->default_variables->n; i++) {
        config_variable_free(vector_get(config->default_variables, i));
    }
    vector_free(config->default_variables);
    for (i=0; i!=config->sections->n; i++) {
        config_section_free(vector_get(config->sections, i));
    }
    vector_free(config->sections);

    if (config->fp != NULL) { // Check if still open
        fclose(config->fp);
    }
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
    int i;
    for (i=0; i!=config->sections->n; i++) {
        if (strcmp(section, vector_get(config->sections, i)) == 0) {
            ConfigSection* current_section = vector_get(config->sections, i);
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