#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "dependency.h"
#include "use.h"

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)  {\
    result = language_get_next(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
}

typedef enum {
    LANGUAGE_IGNORE,
    LANGUAGE_DEPEND,
    LANGUAGE_REQUIRED_USE,
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
} lang_YYBUFFERPOS;

Dependency* depend_parse(const char* buffer);
RequiredUse* required_use_parse(const char* buffer);
Atom* atom_parse(const char* buffer);

void language_init_new(lang_YYBUFFERPOS* buffer, type_delim_t delim);
void language_increment_line(void);
type_delim_t language_get_type_delim(void);
void language_begin_token(const char* t, const char* token_name);
extern void language_print_error(const char *s, ...);
int language_get_next(char* b, int maxBuffer);

#endif // HACKSAW_COMPILER_SHARE_H
