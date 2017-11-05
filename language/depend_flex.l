%option prefix="depend"

%{
#include "depend_bison.h"
#include <stdio.h>
#include <depend.h>

int dependerror(char *s)
{
    fprintf(stderr, "%s in line %d near '%s'\n", s, dependlineno, dependtext);
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
atom    {letter}({letter}|{digit}|[\-\.\/_])*

%%

[ \n\t\r\\]+             ;
"-"                     {return '-';}
"+"                     {return '+';}
"="                     {return '=';}
"!"                     {return '!';}

"^^"                    {dependlval.use = new_use (dependtext, EXACTLY_ONE); return(USE);}
"||"                    {dependlval.use = new_use (dependtext, AT_LEAST_ONE); return(USE);}
"??"                    {dependlval.use = new_use (dependtext, AT_MOST_ONE); return(USE);}

"!"{letter}*"?"         {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (&dependtext[1], NO_USE); 
                            return(USE);
                        }
{letter}*"?"            {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (&dependtext[0], YES_USE); 
                            return(USE);
                        }

"!!"                    {dependlval.block = HARD_BLOCK; return (BLOCKS);}
"!"                     {dependlval.block = SOFT_BLOCK; return (BLOCKS);}

"<="                    {dependlval.version = LESS_E; return (VERSION);} 
">="                    {dependlval.version = GREAT_E; return (VERSION);} 
"<"                     {dependlval.version = LESS; return (VERSION);} 
">"                     {dependlval.version = GREAT; return (VERSION);} 
"="                     {dependlval.version = EQUAL; return (VERSION);} 
"~"                     {dependlval.version = REVISION; return (VERSION);} 

{atom}                  {dependlval.atom_str = strdup (dependtext); return (ATOM);}
[:alpha:]*              {dependlval.atom_str = strdup (dependtext); return (ATOM_USE);}


"("                     {return ('(');}
")"                     {return (')');}
"["                     {return ('[');}
"]"                     {return (']');}
","                     {return (',');}
[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", dependtext);}

%%