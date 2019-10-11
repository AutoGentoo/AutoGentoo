%option prefix="mc"

%{
#define _GNU_SOURCE
#include "make_conf.tab.h"
#include <stdio.h>
#include <share.h>
#include <string.h>

int mcerror(char *s) {
    fprintf(stderr, "%s in line %d near '%s'\n", s, mclineno, mctext);
    exit(1);
}

Map* mc_parse (FILE* fp) {
    mcin = fp;
    mcparse();
    return (Map*)mcout;
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
{ident_1}|{ident_2}    {
                            size_t n = strlen(mctext);
                            mclval.identifier = strndup(mctext + 1, n - 2);
                            return IDENTIFIER;
                        }

{equals}                {return EQUALS;}

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", mctext);}

%%