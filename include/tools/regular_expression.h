//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_REGULAR_EXPRESSION_H_H
#define HACKSAW_REGULAR_EXPRESSION_H_H

#include <tools/string_vector.h>

int re_match (char* string, char* pattern);

// Creates a vector of all the group matches
int re_group_match (StringVector* vec, char* string, char* pattern, int max_groups);

// Looks for the first match and copies it to dest
int re_group_get (char* dest, char* string, char* pattern);

#endif //HACKSAW_REGULAR_EXPRESSION_H_H
