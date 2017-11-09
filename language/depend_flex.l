%option prefix="depend"

%{
#include "depend_bison.h"
#include <stdio.h>
#include <depend.h>

int dependerror(char *s)
{
    fprintf(stderr, "%s in line %d near '%s' at column %d\n", s, dependlineno, dependtext, dependlloc.first_column);
}

DependExpression* depend_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    depend_switch_to_buffer(bs);
    dependparse();
    return (DependExpression*)dependout;
}

%}

letter  [a-zA-Z]
digit   [0-9]
flag    {letter}({letter}|"_"|{digit})*
atom    ({letter}|{digit}|"_"|"."|"+")+

%%

"-"                     {return '-';}
"/"                     {return '/';}
"+"                     {return '+';}
"="                     {return '=';}
"!"                     {return '!';}
"?"                     {return '?';}
":"                     {return ':';}
"("                     {return ('(');}
")"                     {return (')');}
"["                     {return ('[');}
"]"                     {return (']');}
","                     {return (',');}

"^^"                    {dependlval.use = new_use (dependtext, EXACTLY_ONE); return(USE);}
"||"                    {dependlval.use = new_use (dependtext, AT_LEAST_ONE); return(USE);}
"??"                    {dependlval.use = new_use (dependtext, AT_MOST_ONE); return(USE);}

"!"{flag}"?"            {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (&dependtext[1], NO_USE); 
                            return(USE);
                        }
{flag}"?"               {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (dependtext, YES_USE); 
                            return(USE);
                        }

"!!"                    {dependlval.block = HARD_BLOCK; return (BLOCKS);}

"<="                    {dependlval.version = LESS_E; return (VERSION);} 
">="                    {dependlval.version = GREAT_E; return (VERSION);} 
"<"                     {dependlval.version = LESS; return (VERSION);} 
">"                     {dependlval.version = GREAT; return (VERSION);} 
"="                     {dependlval.version = EQUAL; return (VERSION);} 
"~"                     {dependlval.version = DEP_REVISION; return (VERSION);} 

{atom}                  {dependlval.atom_str = strdup (dependtext); return (IDENT);}
[ \n\t\r\\]+            ;

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", dependtext);}

%%