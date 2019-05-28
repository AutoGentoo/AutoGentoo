//
// Created by atuser on 5/26/19.
//

#ifndef AUTOGENTOO_SUGGESTION_H
#define AUTOGENTOO_SUGGESTION_H

#include "constants.h"
#include "atom.h"

struct __Suggestion {
	char* required_by;
	char* portage_file;
	char* line_addition;
	
	Suggestion* next;
};

void emerge_suggestion_keyword(Emerge* emerge, Ebuild* required_by, P_Atom* selector);

#endif //AUTOGENTOO_SUGGESTION_H
