//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_REGULAR_EXPRESSION_H_H
#define HACKSAW_REGULAR_EXPRESSION_H_H

#include <tools/vector.h>

int re_match (char* string, char* pattern);
void re_group_match (Vector** vec, char* string, char* pattern, int max_groups);

#endif //HACKSAW_REGULAR_EXPRESSION_H_H
