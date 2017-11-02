%{
#include <stdio.h>

void yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
}

int sym[26];
%}

%union {
  int number;
  int var;
};
%token NO_USE YES_USE
%token EXACT_ONE LEAST_ONE MOST_ONE
%token ATOM LE_ATOM GE_ATOM L_ATOM G_ATOM E_ATOM

%%

expr : use '(' expr ')'
     | select expr
     | expr select
     | select
     ;

use : NO_USE
    | YES_USE
    | EXACT_ONE
    | LEAST_ONE
    | MOST_ONE
    ;

select  : ATOM
        | LE_ATOM
        | GE_ATOM
        | L_ATOM
        | G_ATOM
        | E_ATOM
        ;