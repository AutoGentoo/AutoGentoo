//
// Created by atuser on 12/31/19.
//

#ifndef AUTOGENTOO_CMP_H
#define AUTOGENTOO_CMP_H

#include "../atom.h"

int pd_compare_range(int cmp, atom_version_t range);
int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom);

#endif //AUTOGENTOO_CMP_H
