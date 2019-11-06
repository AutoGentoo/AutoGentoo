//
// Created by atuser on 10/29/19.
//

#ifndef AUTOGENTOO_BACKTRACK_H
#define AUTOGENTOO_BACKTRACK_H

#include <autogentoo/hacksaw/vector.h>
#include "constants.h"

void backtrack_selected_ebuild(Emerge* em, Vector* dependency_ordered, SelectedEbuild* se);

#endif //AUTOGENTOO_BACKTRACK_H
