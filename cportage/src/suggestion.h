//
// Created by atuser on 5/26/19.
//

#ifndef AUTOGENTOO_SUGGESTION_H
#define AUTOGENTOO_SUGGESTION_H

#include <stdio.h>
#include "constants.h"
#include "atom.h"

struct __Suggestion {
    char* required_by;
    char* line_addition;

    Suggestion* next;
};

Suggestion* suggestion_new(char* required_by, char* line, ...);

FILE* suggestion_read(Suggestion* s);

void emerge_apply_suggestions(Emerge* em);

#endif //AUTOGENTOO_SUGGESTION_H
