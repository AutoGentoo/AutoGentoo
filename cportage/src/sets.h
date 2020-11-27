//
// Created by atuser on 10/28/19.
//

#ifndef AUTOGENTOO_SETS_H
#define AUTOGENTOO_SETS_H

#include <autogentoo/hacksaw/vector.h>
#include "atom.h"

struct __PortageSet {
    char* name;
    Vector* atoms;
};

PortageSet* portage_set_new(char* atoms);

Dependency* portage_set_expand(PortageSet* set);

PortageSet* portage_set_selected_packages(Emerge* em);

PortageSet* portage_set_selected_sets(Emerge* em);


#endif //AUTOGENTOO_SETS_H
