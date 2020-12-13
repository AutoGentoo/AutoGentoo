%option prefix="requireduse"

%{
#include <stdio.h>
#include "language.h"
#include <string.h>
#include "requireduse.tab.h"
#include <assert.h>

#include <signal.h>

int asprintf(char** strp, const char* fmt, ...);

#define YY_INPUT(buf,result,max_size)  {\
    result = language_get_next(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
}

%}

letter          [a-zA-Z]
digit           [0-9]
identifier      ({letter}|{digit}|_|\+|\*|\-)+

%%
[\x01]                  {
                            language_begin_token(requiredusetext, "delim");
                            switch(language_get_type_delim())
                            {
                                case LANGUAGE_REQUIRED_USE: return REQUIRED_USE;
                                case LANGUAGE_IGNORE:
                                default:
                                    break;
                            }
                        }
[\n]                    {
                            language_begin_token(requiredusetext, "newline");
                            language_increment_line();
                        }
[ \t\r\\]+              {language_begin_token(requiredusetext, "tab");};
"??"                    {requireduselval.use_select.target = NULL; language_begin_token(requiredusetext, NULL); requireduselval.use_select.operator = USE_OP_MOST_ONE; return USESELECT;}
"||"                    {requireduselval.use_select.target = NULL; language_begin_token(requiredusetext, NULL); requireduselval.use_select.operator = USE_OP_LEAST_ONE; return USESELECT;}
"^^"                    {requireduselval.use_select.target = NULL; language_begin_token(requiredusetext, NULL); requireduselval.use_select.operator = USE_OP_EXACT_ONE; return USESELECT;}
"(+)"                   {requireduselval.use_default = ATOM_DEFAULT_ON; language_begin_token(requiredusetext, NULL); return USE_DEFAULT;}
"(-)"                   {requireduselval.use_default = ATOM_DEFAULT_OFF; language_begin_token(requiredusetext, NULL); return USE_DEFAULT;}
"-"                     {language_begin_token(requiredusetext, NULL); return '-';}
"<"                     {language_begin_token(requiredusetext, NULL); return '<';}
">"                     {language_begin_token(requiredusetext, NULL); return '>';}
"="                     {language_begin_token(requiredusetext, NULL); return '=';}
"!"                     {language_begin_token(requiredusetext, NULL); return '!';}
[\[]                    {language_begin_token(requiredusetext, NULL); return '[';}
[\]]                    {language_begin_token(requiredusetext, NULL); return ']';}
[\?]                    {language_begin_token(requiredusetext, NULL); return '?';}
[\(]                    {language_begin_token(requiredusetext, NULL); return '(';}
[\)]                    {language_begin_token(requiredusetext, NULL); return ')';}
[\^]                    {language_begin_token(requiredusetext, NULL); return '^';}
[\|]                    {language_begin_token(requiredusetext, NULL); return '|';}
[\,]                    {language_begin_token(requiredusetext, NULL); return ',';}
[\~]                    {language_begin_token(requiredusetext, NULL); return '~';}
{identifier}            {
                            char* ref = strdup(requiredusetext);
                            requireduselval.identifier = ref;
                            language_begin_token(ref, "identifier");
                            return IDENTIFIER;
                        }
[<<EOF>>|'']            {language_begin_token(requiredusetext, "eof"); return END_OF_FILE;}
.                       {language_begin_token(requiredusetext, "invalid"); language_print_error("Unknown character: %s", requiredusetext);}

%%