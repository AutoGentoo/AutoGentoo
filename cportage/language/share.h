#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "../package.h"
#include "../use.h"

extern int indent;
extern int error;

Dependency* depend_parse(char* buffer);
RequiredUse* required_use_parse (char* buffer);
P_Atom* atom_parse (char* buffer);
Map* mc_parse (FILE* fp);

#endif // HACKSAW_COMPILER_SHARE_H