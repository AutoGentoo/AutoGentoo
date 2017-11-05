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

%locations
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
    AtomSlot slot;
}

%token <use> USE
%token <atom_str> IDENT

%token <block> BLOCKS
%token <version> VERSION

%token END_OF_FILE

%type <dependexpression> expr
%type <atom> select
%type <atom_opts> opts
%type <vec> req_use
%type <use_req> use_flag
%type <use_req> use_token;
%type <use_req> default_use_flag;
%type <slot> slot
%type <block> block
%type <atom> useselect

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
                                            Vector* ar = vector_new (sizeof (DependExpression),
                                                                    REMOVE | UNORDERED);
                                            vector_add(ar, $1);
                                            vector_add(ar, $2);
                                            $$ = new_dependexpression (ar, EXPR_EXPR);
                                        }
        | useselect                     {
                                            $$ = new_dependexpression($1, SEL_EXPR);
                                        }
        ;


useselect : select
            | select '[' req_use ']'    {
                                            $$->opts.required_use = $3;
                                        }

select :    opts IDENT '/' IDENT        {
                                            char current[128];
                                            sprintf (current, "%s/%s", $2, $4);
                                            $$ = new_atom(current, $1);
                                            $$->slot.rebuild = -1; // Disable slot checking
                                        }
            | opts IDENT '/' IDENT slot {
                                            char current[128];
                                            sprintf (current, "%s/%s", $2, $4);
                                            $$ = new_atom(current, $1);
                                            $$->slot = $5;
                                        }
            ;

slot :  ':' '='                         {
                                            $$.rebuild = SLOT_REBUILD;
                                            $$.main_slot = NULL;
                                            $$.sub_slot = NULL;
                                        }
        | ':' '*'                       {
                                            $$.rebuild = SLOT_NO_REBUILD;
                                            $$.main_slot = NULL;
                                            $$.sub_slot = NULL;
                                        }
        | ':' IDENT                     {
                                            $$.rebuild = SLOT_NO_REBUILD;
                                            $$.main_slot = strdup ($2);
                                            $$.sub_slot = NULL;
                                        }
        | ':' IDENT '='                     {
                                            $$.rebuild = SLOT_REBUILD;
                                            $$.main_slot = strdup ($2);
                                            $$.sub_slot = NULL;
                                        }
        | ':' IDENT '/' IDENT           {
                                            $$.rebuild = SLOT_REBUILD;
                                            $$.main_slot = strdup ($2);
                                            $$.sub_slot = strdup ($4);
                                        }
        ;

req_use :   req_use ',' req_use         {
                                            vector_extend($1, $3);
                                            $$ = $1;
                                            vector_free ($3);
                                        }
            | default_use_flag '?'      {
                                            $$ = vector_new (sizeof(RequireUse), REMOVE | UNORDERED);
                                            if ($1.status == DISABLED) {
                                                $1.status = OPPOSOTE_CHECK;
                                            }
                                            else {
                                                $1.status = CHECK;
                                            }
                                            vector_add($$, &$1);
                                        }
            | use_flag                  {
                                            $$ = vector_new (sizeof(RequireUse), REMOVE | UNORDERED);
                                            vector_add($$, &$1);
                                        }
            | default_use_flag          {
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

use_flag :  use_token                   {
                                            $$ = $1;
                                        }
            | '-' use_token             {
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
            ;

default_use_flag :  use_token '(' '-' ')'       {
                                                    $$ = $1;
                                                    $$._default = DEFAULT_DISABLE;
                                                }
                    | use_token '(' '+' ')'     {
                                                    $$ = $1;
                                                    $$._default = DEFAULT_ENABLE;
                                                }
                    | '-' default_use_flag      {
                                                    $$ = $2;
                                                    $$.status = DISABLED;
                                                }
                    ;

use_token : IDENT                       {
                                            $$.flag = strdup ($1);
                                            $$.status = ENABLED;
                                            $$._default = DEFAULT_NONE;
                                        }
            ;

opts :                                  {
                                            set_atom_opts(&$$, ALL, NO_BLOCK); 
                                        } 
        | block VERSION                 {
                                            set_atom_opts(&$$, $2, $1);
                                        }
        | VERSION                       {
                                            set_atom_opts(&$$, $1, NO_BLOCK);
                                        }
        | block                         {
                                            set_atom_opts(&$$, ALL, $1);
                                        }
        ;

block : '!'                             {$$ = SOFT_BLOCK;}
        | BLOCKS                        {$$ = HARD_BLOCK;}
        ;