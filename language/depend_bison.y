%define api.prefix {depend}

%{
#include <stdio.h>

int dependparse(void);
int dependwrap() { return 1; }
int dependlex();
extern int dependlineno;
extern char* dependtext;

void dependerror(const char *message);
%}

%code requires {
  #include "depend.h"
  #include <stdlib.h>
}

%start program

%union {
    Atom* atom;
    DependExpression* dependexpression;
    Use* use;
}

%token <use> NO_USE
%token <use> YES_USE
%token <use> EXACT_ONE
%token <use> LEAST_ONE
%token <use> MOST_ONE

%token <atom> ATOM 
%token <atom> LE_ATOM
%token <atom> GE_ATOM
%token <atom> L_ATOM
%token <atom> G_ATOM
%token <atom> E_ATOM

%token END_OF_FILE

%type <dependexpression> expr
%type <atom> select
%type <use> use

%%

program:    | expr {debug_dependexpression($1);}
            | END_OF_FILE {printf("End\n");}
            ;

expr :  use[out] '(' expr[in] ')'       {
                                            $$ = new_dependexpression(
                                                new_check_use($out, $in), 
                                                USE_EXPR
                                            );
                                        }
        | expr expr                     {
                                            DependExpression* ar[] = {
                                                $1,
                                                $2
                                            };
                                            $$ = new_dependexpression (ar, EXPR_EXPR);
                                        }
        | select                        {
                                            $$ = new_dependexpression($1, SEL_EXPR);
                                        }
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