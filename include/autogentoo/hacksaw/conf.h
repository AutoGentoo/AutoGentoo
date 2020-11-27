//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_CONFIG_H
#define HACKSAW_CONFIG_H

#include "string_vector.h"
#include "map.h"

typedef struct __Conf Conf;
typedef struct __ConfSection ConfSection;
typedef struct __ConfVariable ConfVariable;

struct __Conf {
    char* path;
    StringVector* sections;
    Map* variables; // Key: [sectionName](variableName)
};

Conf* conf_new(char* path);

void conf_free(Conf* conf);

char* conf_get(Conf* conf, char* section, char* variable_name);

int conf_get_convert(Conf* conf, char* dest, char* section, char* variable_name);

StringVector* conf_get_vector(Conf* conf, char* section, char* variable_name);

#endif //HACKSAW_CONFIG_H_H
