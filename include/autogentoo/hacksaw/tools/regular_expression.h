//
// Created by atuser on 10/21/17.
//

#ifndef HACKSAW_REGULAR_EXPRESSION_H
#define HACKSAW_REGULAR_EXPRESSION_H

#include "string_vector.h"

typedef struct __RegexMatch RegexMatch;

struct __RegexMatch {
	char* name;
	char* match;
	size_t length;
};

RegexMatch* regex_full(const char* value, char* pattern);

int regex_simple(char* dest, char* string, char* pattern);

#endif //HACKSAW_REGULAR_EXPRESSION_H_H
