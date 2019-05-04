#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "../package.h"

extern int indent;
extern int error;

Dependency* depend_parse(char* buffer);

#endif // HACKSAW_COMPILER_SHARE_H