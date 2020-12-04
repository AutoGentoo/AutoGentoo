//
// Created by tumbar on 12/3/20.
//

#include <stdio.h>
#include <string.h>
#include "language.h"
#include "depend.tab.h"

#ifndef LANGUAGE_DEBUG
#define LANGUAGE_DEBUG 0
#endif

extern void* yyout;

extern int yylex(void);

static __thread YYBUFFERPOS* buffer_pos;
static int yy_error_ag_0_ = 0;

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

extern YYLTYPE yylloc;


void language_print_error(const char* errorstring, ...)
{
    static char errmsg[10000];
    va_list args;

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
    vsprintf(errmsg, errorstring, args);
    va_end(args);

    fprintf(stdout, "Error: %s\n", errmsg);
}

void language_init_new(YYBUFFERPOS* buffer, type_delim_t delim)
{
    buffer_pos = buffer;
    memset(buffer_pos, 0, sizeof(YYBUFFERPOS));
    buffer_pos->delim = delim;
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
    yylloc.first_line = buffer_pos->nRow;
    yylloc.first_column = buffer_pos->nTokenStart;
    yylloc.last_line = buffer_pos->nRow;
    yylloc.last_column = buffer_pos->nTokenStart + buffer_pos->nTokenLength - 1;

#if LANGUAGE_DEBUG
    if (!token_name)
        printf("Token '%s' at %d:%d next at %d\n", dumpString(t),
                yylloc.first_column,
                yylloc.last_column, buffer_pos->nTokenNextStart);
    else
        printf("Token '%s' at %d:%d next at %d (%s)\n", dumpString(t),
               yylloc.first_column,
               yylloc.last_column, buffer_pos->nTokenNextStart,
               token_name);
    fflush(stdout);
#endif
}

void language_increment_line()
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

inline type_delim_t language_get_type_delim()
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

int yyerror(const char* s)
{
    yy_error_ag_0_ = 1;
    language_print_error(s);
    return error;
}

Dependency* depend_parse(const char* buffer)
{
    YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_DEPEND);
    language_feed_string(buffer);
    yyparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(yyout);
        return NULL;
    }

    return (Dependency*) yyout;
}

Dependency* cmdline_parse(const char* buffer)
{
    yy_error_ag_0_ = 0;
    YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_CMDLINE);
    language_feed_string(buffer);
    yyparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(yyout);
        return NULL;
    }

    return (Dependency*) yyout;
}

Atom* atom_parse(const char* buffer)
{
    yy_error_ag_0_ = 0;
    YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_DEPEND);
    language_feed_string(buffer);
    yyparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(yyout);
        return NULL;
    }

    Atom* atom_out = ((Dependency*) yyout)->atom;
    Py_INCREF(atom_out);
    Py_DECREF(yyout);

    return atom_out;
}

RequiredUse* required_use_parse(const char* buffer)
{
    yy_error_ag_0_ = 0;
    YYBUFFERPOS pos;
    language_init_new(&pos, LANGUAGE_REQUIRED_USE);
    language_feed_string(buffer);
    yyparse();

    if (yy_error_ag_0_)
    {
        Py_XDECREF(yyout);
        return NULL;
    }

    return (RequiredUse*) yyout;
}
