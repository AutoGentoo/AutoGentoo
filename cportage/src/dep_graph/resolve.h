//
// Created by atuser on 12/31/19.
//

#ifndef AUTOGENTOO_RESOLVE_H
#define AUTOGENTOO_RESOLVE_H

#include "../atom.h"

int pd_compare_range(int cmp, atom_version_t range);
int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom);
Package* package_resolve_atom(Emerge* em, P_Atom* atom);

#endif //AUTOGENTOO_RESOLVE_H
