//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_CONFIG_H
#define HACKSAW_CONFIG_H

#include "string_vector.h"

typedef struct __Conf Conf;
typedef struct __ConfSection ConfSection;
typedef struct __ConfVariable ConfVariable;

struct __Conf {
    char* path;
    Vector* sections;
    Vector* default_variables;
};

struct __ConfVariable {
    Conf* parent;
    ConfSection* parent_section;
    char* identifier;
    char* value;
};

struct __ConfSection {
    Conf* parent;
    char* name;
    Vector* variables;
};

Conf* conf_new (char* path);
void conf_section_read (Conf* conf, ConfSection* section, Vector* variables, long start, long stop, FILE* fp);
ConfSection* conf_section_new (char* name);
void conf_variable_new (Conf* conf, ConfSection* section, ConfVariable* var, StringVector* data);
void conf_free (Conf* conf);
void conf_section_free (ConfSection* section);
void conf_variable_free (ConfVariable* var);
char* conf_get (Conf* conf, char* section, char* variable_name);
int conf_get_convert (Conf* conf, char* dest, char* section, char* variable_name);
StringVector* conf_get_vector (Conf* conf, char* section, char* variable_name);

#endif //HACKSAW_CONFIG_H_H
