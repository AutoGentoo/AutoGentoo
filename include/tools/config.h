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
    Vector* sections;
    Vector* default_variables;
};

struct __ConfigVariable {
    Config* parent;
    ConfigSection* parent_section;
    char identifier[64];
    char* value;
};

struct __ConfigSection {
    Config* parent;
    char name[64];
    Vector* variables;
};

Config* config_read (char* path);
void config_section_read (Config* config, ConfigSection* section, Vector* variables, long start, long stop, FILE* fp);
ConfigSection* config_section_new (char* name);
void config_variable_new (Config* config, ConfigSection* section, ConfigVariable* var, StringVector* data);
void config_free (Config* config);
void config_section_free (ConfigSection* section);
void config_variable_free (ConfigVariable* var);
char* config_get (Config* config, char* section, char* variable_name);
int config_get_convert (Config* config, char** dest, char* section, char* variable_name);
StringVector* config_get_vector (Config* config, char* section, char* variable_name);

#endif //HACKSAW_CONFIG_H_H
