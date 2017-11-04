%define api.prefix {depend}

%code requires {
  #include "depend.h"
  #include <stdlib.h>
}

%{
#include <stdio.h>
#include <depend.h>

int dependparse(void);
int dependwrap() { return 1; }
int dependlex();
extern int dependlineno;
extern char* dependtext;
extern DependExpression* dependout;

void dependerror(const char *message);
%}

%start program

%union {
    Atom* atom;
    char* atom_str;
    AtomOpts atom_opts;
    DependExpression* dependexpression;
    Use* use;
    block_t block;
    atom_t version;
}

%token <use> USE

%token <atom_str> ATOM

%token <block> BLOCKS
%token <version> VERSION

%token END_OF_FILE

%type <dependexpression> expr
%type <atom> select
%type <atom_opts> opts

%%

program:    | expr  {
                        dependout = $1;
                    }
            | END_OF_FILE
            ;

expr :  USE[out] '(' expr[in] ')'       {
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

select :    opts ATOM                   {
                                            $$ = new_atom($2, $1);
                                        }

opts :                                  {
                                            set_atom_opts(&$$, ALL, NO_BLOCK); 
                                        }
        | BLOCKS VERSION                {
                                            set_atom_opts(&$$, $2, $1);
                                        }
        | VERSION                       {
                                            set_atom_opts(&$$, $1, NO_BLOCK);
                                        }
        | BLOCKS                        {
                                            set_atom_opts(&$$, ALL, $1);
                                        }