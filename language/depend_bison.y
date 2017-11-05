%define api.prefix {depend}

%code requires {
  #include "depend.h"
  #include <stdlib.h>
  #include <string.h>
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
    RequireUse use_req;
    Vector* vec;
}

%token <use> USE
%token <atom_str> ATOM_USE
%token <atom_str> ATOM

%token <block> BLOCKS
%token <version> VERSION

%token END_OF_FILE

%type <dependexpression> expr
%type <atom> select
%type <atom_opts> opts
%type <vec> req_use
%type <use_req> use_flag
%type <use_req> use_token;

%%

program:    | expr                      {
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
            | select '[' req_use ']'    {
                                            $$->opts.required_use = $3;
                                        }
            ;

req_use :   req_use ',' req_use         {
                                            vector_extend($1, $3);
                                            $$ = $1;
                                            vector_free ($3);
                                        }
            | use_flag                  {
                                            $$ = vector_new (sizeof(RequireUse), REMOVE | UNORDERED);
                                            vector_add($$, &$1);
                                        }
            | USE                       {
                                            RequireUse temp;
                                            temp.flag = strdup($1->str);
                                            if ($1->type == YES_USE) {
                                                temp.status = CHECK;
                                            }
                                            else if ($1->type == NO_USE) {
                                                temp.status = OPPOSOTE_CHECK;
                                            }
                                            else {
                                                dependerror("Invalid compact use");
                                            }
                                            $$ = vector_new (sizeof(RequireUse), REMOVE | UNORDERED);
                                            vector_add($$, &temp);
                                        }
            ;

use_flag : '-' use_token                {
                                            $$ = $2;
                                            $$.status = DISABLED;
                                        }
            | use_token '='             {
                                            $$ = $1;
                                            $$.status = SAME;
                                        }
            | '!' use_token '='         {
                                            $$ = $2;
                                            $$.status = OPPOSITE;
                                        }
            | use_token '(' '-' ')'     {
                                            $$ = $1;
                                            $$.status = DEFAULT_DISABLE;
                                        }
            | use_token '(' '+' ')'     {
                                            $$ = $1;
                                            $$.status = DEFAULT_ENABLE;
                                        }
            ;

use_token :   ATOM_USE                  {
                                            $$.flag = strdup ($1);
                                            $$.status = ENABLED;
                                        }
            | ATOM                      {
                                            $$.flag = strdup ($1);
                                            $$.status = ENABLED;
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
        ;