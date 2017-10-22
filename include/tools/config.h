//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_CONFIG_H_H
#define HACKSAW_CONFIG_H_H

#include <tools/string_vector.h>

typedef struct __Config Config;
typedef struct __ConfigSection ConfigSection;
typedef struct __ConfigVariable ConfigVariable;

struct __Config {
    char path[256];
    FILE* fp;
    Vector* sections;
    Vector* default_variables;
};

struct __ConfigVariable {
    char identifier[64];
    char* value;
};

struct __ConfigSection {
    char name[64];
    Vector* variables;
};

Config* config_read (char* config_path);
ConfigSection* config_section_new (char* name);
void config_variable_new (ConfigVariable* var, StringVector* data);
void config_free (Config* config);
void config_section_free (ConfigSection* section);
void config_variable_free (ConfigVariable* var);
char* config_get (Config* config, char* section, char* variable_name);

#endif //HACKSAW_CONFIG_H_H
