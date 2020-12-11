#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "use.h"
#include "dependency.h"

typedef enum {
    LANGUAGE_IGNORE,
    LANGUAGE_DEPEND,
    LANGUAGE_REQUIRED_USE,
    LANGUAGE_CMDLINE
} type_delim_t;

typedef struct
{
    const char* buffer;

    type_delim_t delim;
    int nRow;
    int nBuffer;
    int lBuffer;
    int nTokenStart;
    int nTokenLength;
    int nTokenNextStart;
} YYBUFFERPOS;

Dependency* depend_parse(const char* buffer);
RequiredUse* required_use_parse(const char* buffer);
Atom* atom_parse(const char* buffer);
Dependency* cmdline_parse(const char* buffer);

void language_init_new(YYBUFFERPOS* buffer, type_delim_t delim);
void language_increment_line(void);
type_delim_t language_get_type_delim(void);
void language_begin_token(const char* t, const char* token_name);
extern void language_print_error(const char *s, ...);
int language_get_next(char* b, int maxBuffer);

#endif // HACKSAW_COMPILER_SHARE_H
