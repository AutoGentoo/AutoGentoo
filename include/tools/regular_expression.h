//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_REGULAR_EXPRESSION_H_H
#define HACKSAW_REGULAR_EXPRESSION_H_H

#include <tools/string_vector.h>

Vector* regex_full (char* value, char* pattern);
int regex_simple (char* dest, char* string, char* pattern);

#endif //HACKSAW_REGULAR_EXPRESSION_H_H
