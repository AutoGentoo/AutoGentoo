%{
#include <stdio.h>

int yyparse(void);
int yywrap() { return 1; }
extern int yylineno;
extern char* yytext;

void yyerror(const char *message);
%}

%code requires {
  #include "use.h"
  #include <stdlib.h>
}

%start program

%union {
    Atom* atom;
    Expression* expression;
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

%type <expression> expr
%type <atom> select
%type <use> use

%%

program:    | expr {debug_expression($1);}
            | END_OF_FILE {printf("End\n");}
            ;

expr :  use[out] '(' expr[in] ')'       {
                                            $$ = new_expression(
                                                new_check_use($out, $in), 
                                                USE_EXPR
                                            );
                                        }
        | expr expr                     {
                                            Expression* ar[] = {
                                                $1,
                                                $2
                                            };
                                            $$ = new_expression (ar, EXPR_EXPR);
                                        }
        | select                        {
                                            $$ = new_expression($1, SEL_EXPR);
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