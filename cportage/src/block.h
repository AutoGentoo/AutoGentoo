//
// Created by atuser on 12/16/19.
//

#ifndef CPORTAGE_BLOCK_H
#define CPORTAGE_BLOCK_H

#include "resolve.h"
#include "use.h"
#include "portage_log.h"

int resolved_ebuild_blocked(ResolvedEbuild* e, P_Atom* block_atom, UseFlag* use);

#endif //CPORTAGE_BLOCK_H
