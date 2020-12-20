//
// Created by tumbar on 12/3/20.
//

#include <stdio.h>
#include <string.h>
#include "language.h"

#ifndef LANGUAGE_DEBUG
#define LANGUAGE_DEBUG 0
#endif

extern void* dependout;
extern void* requireduseout;

static __thread lang_YYBUFFERPOS* buffer_pos;
static int yy_error_ag_0_ = 0;

void dependparse(void);
void requireduseparse(void);

/* Location type.  */
#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
#define YYLTYPE_IS_DECLARED 1
#endif

extern YYLTYPE dependlloc;
extern YYLTYPE requireduselloc;

__thread type_delim_t current_parser_type;


void language_print_error(const char* errorstring, ...)
{
    static char errmsg[10000];
    va_list args;
    yy_error_ag_0_ = 1;

    int start = buffer_pos->nTokenStart;
    int end = start + buffer_pos->nTokenLength - 1;
    int i;

    fprintf(stdout, "       |%s\n", buffer_pos->buffer + 1);
    if (buffer_pos->nBuffer >= buffer_pos->lBuffer)
    {
        fprintf(stdout, "...... !");
        for (i = 0; i < buffer_pos->lBuffer; i++)
            fprintf(stdout, ".");
        fprintf(stdout, "^-EOF\n");
    } else
    {
        fprintf(stdout, "...... !");
        for (i = 1; i < start; i++)
            fprintf(stdout, ".");
        for (i = start; i <= end; i++)
            fprintf(stdout, "^");
        for (i = end + 1; i < buffer_pos->lBuffer; i++)
            fprintf(stdout, ".");
        fprintf(stdout, "   token%d:%d\n", start, end);
    }

    va_start(args, errorstring);
    vprintf(errorstring, args);
    va_end(args);

    fprintf(stdout, "Error: %s\n", errmsg);
}

void language_init_new(lang_YYBUFFERPOS* buffer, type_delim_t delim)
{
    buffer_pos = buffer;
    memset(buffer_pos, 0, sizeof(lang_YYBUFFERPOS));
    buffer_pos->delim = delim;
    current_parser_type = delim;
}

static
char dumpChar(char c)
{
    if (isprint(c))
        return c;
    return '@';
}

/*--------------------------------------------------------------------
 * dumpString
 *
 * printable version of a string upto 100 character
 *------------------------------------------------------------------*/
static
char* dumpString(const char* s)
{
    static char buf[101];
    int i;
    int n = (int) strlen(s);

    if (n > 100)
        n = 100;

    for (i = 0; i < n; i++)
        buf[i] = dumpChar(s[i]);
    buf[i] = 0;
    return buf;
}

void language_begin_token(const char* t, const char* token_name)
{
    /*================================================================*/
    /* remember last read token --------------------------------------*/
    buffer_pos->nTokenStart = buffer_pos->nTokenNextStart;
    buffer_pos->nTokenLength = (int) strlen(t);
    buffer_pos->nTokenNextStart = buffer_pos->nBuffer;

    /*================================================================*/
    /* location for bison --------------------------------------------*/
    YYLTYPE* pos = NULL;
    if (current_parser_type == LANGUAGE_DEPEND)
        pos = &dependlloc;
    else
        pos = &requireduselloc;

    pos->first_line = buffer_pos->nRow;
    pos->first_column = buffer_pos->nTokenStart;
    pos->last_line = buffer_pos->nRow;
    pos->last_column = buffer_pos->nTokenStart + buffer_pos->nTokenLength - 1;

#if LANGUAGE_DEBUG
    if (!token_name)
        printf("Token '%s' at %d:%d next at %d  # %d\n", dumpString(t),
                pos->first_column,
               pos->last_column, buffer_pos->nTokenNextStart, current_parser_type);
    else
        printf("Token '%s' at %d:%d next at %d (%s)  # %d\n", dumpString(t),
               pos->first_column,
               pos->last_column, buffer_pos->nTokenNextStart,
               token_name, current_parser_type);
    fflush(stdout);
#endif
}

void language_increment_line(void)
{
    buffer_pos->nRow++;
}

static void language_feed_string(const char* input)
{
    buffer_pos->buffer = input - 1;
    buffer_pos->nTokenStart = -1;
    buffer_pos->nTokenNextStart = 1;
    buffer_pos->nBuffer = 0;
    buffer_pos->lBuffer = (int) strlen(input) + 1;
}

inline type_delim_t language_get_type_delim(void)
{
    return buffer_pos->delim;
}

inline int language_get_next(char* b, int maxBuffer)
{
    if (!buffer_pos->nBuffer)
    {
        b[0] = 1;
        buffer_pos->nBuffer++;
        return 1;
    }

    if (buffer_pos->nBuffer >= buffer_pos->lBuffer)
        return 0;

    b[0] = buffer_pos->buffer[buffer_pos->nBuffer++];
    return b[0] != 0;
}

void requireduseerror(char const *msg)
{
    language_print_error(msg);
}

void dependerror(const char* s)
{
    language_print_error(s);
}

Dependency* depend_parse(const char* buffer)
{
    lang_YYBUFFERPOS pos;
    dependout = NULL;
    language_init_new(&pos, LANGUAGE_DEPEND);
    language_feed_string(buffer);
    dependparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(dependout);
        return NULL;
    }

    return (Dependency*) dependout;
}

Atom* atom_parse(const char* buffer)
{
    yy_error_ag_0_ = 0;
    dependout = NULL;
    lang_YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_DEPEND);
    language_feed_string(buffer);
    dependparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(dependout);
        return NULL;
    }

    Atom* atom_out = ((Dependency*) dependout)->atom;
    Py_INCREF(atom_out);
    Py_DECREF(dependout);

    return atom_out;
}

RequiredUse* required_use_parse(const char* buffer)
{
    yy_error_ag_0_ = 0;
    requireduseout = NULL;
    lang_YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_REQUIRED_USE);
    language_feed_string(buffer);
    requireduseparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(requireduseout);
        return NULL;
    }

    return (RequiredUse*) requireduseout;
}
