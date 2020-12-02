%option prefix="mc"

%{
#include "make_conf.tab.h"
#include <stdio.h>
#include "language.h"
#include <string.h>

int mcerror(char *s) {
    fprintf(stderr, "MAKECONF ERROR %s in line %d near '%s'\n", s, mclineno, mctext);
    exit(1);
}

void make_conf_parse (FILE* fp, Map* out) {
    mcin = fp;
    mcout = (void*)out;
    mcparse();
}

%}

letter          [a-zA-Z]
digit           [0-9]
identifier      ({letter}|{digit}|_|\+|\*|-|\/|\.|\:)+
ident_1         [']([^']*)[']
ident_2         ["]([^"]*)["]
comment         [#]([^\n])*[\n]
equals          [ ]*[\=][ ]*


%%
{comment}               mclineno++;
[\n]                    mclineno++;
[\t\r\\]+               ;
{identifier}            {
                            mclval.identifier = strdup(mctext);
                            return IDENTIFIER;
                        }
{ident_1}|{ident_2}     {
                            size_t n = strlen(mctext);
                            mclval.identifier = strndup(mctext + 1, n - 2);
                            return IDENTIFIER;
                        }

{equals}                {return EQUALS;}

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", mctext);}

%%