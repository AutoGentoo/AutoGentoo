#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include <cportage/portage.h>
#include <cportage/expr.h>

extern int indent;
extern int error;

/* make_conf */


/* depend */
Dependency* depend_parse(const char* buffer);
RequiredUse* required_use_parse(const char* buffer);
Atom* atom_parse(const char* buffer);
Dependency* cmdline_parse(const char* buffer);
void make_conf_parse(FILE* fp, Map* out);

#endif // HACKSAW_COMPILER_SHARE_H