#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "expr.h"

extern int indent;
extern int error;

Dependency* depend_parse(Portage* parent, const char* buffer);
RequiredUse* required_use_parse(Portage* parent, const char* buffer);
Atom* atom_parse(const char* buffer);
Dependency* cmdline_parse(Portage* parent, const char* buffer);
void make_conf_parse(FILE* fp, Map* out);

#endif // HACKSAW_COMPILER_SHARE_H