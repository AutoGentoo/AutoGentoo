#ifndef HACKSAW_COMPILER_SHARE_H
#define HACKSAW_COMPILER_SHARE_H

#include "atom.h"
#include "depend.h"

extern int indent;

void printf_with_indent(char* format, ...);

extern int error;

AtomSelector* atom_parse(char* buffer);

DependExpression* depend_parse(char* buffer);

#endif // HACKSAW_COMPILER_SHARE_H