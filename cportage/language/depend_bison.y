%code requires {
  #include "share.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
#include <stdio.h>
#include "share.h"

int yyparse(void);
int yywrap() { return 1; }
int yylex();
extern int yylineno;
extern char* yytext;
extern void* yyout;

void yyerror(const char *message);
%}

%start program

%union {
    char* identifier;
    P_Atom* atom_type;
    Dependency* depend_type;
    atom_use_default use_default;
    RequiredUse* use_type;
    use_select_t use_select;

    struct {
        char* target;
        use_select_t t;
    } depend_expr_select;

    struct {
        char* name;
        char* sub_name;
    } slot;

    AtomFlag* atom_flag;
}

%token <identifier> IDENTIFIER
%token <identifier> REPOSITORY
%token <atom_type> ATOM
%token END_OF_FILE
%token <use_default> USE_DEFAULT;
%token <slot> SLOT
%token <use_select> USESELECT

%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot_rebuild
%type <atom_type> atom_slot
%type <atom_type> atom_repo
%type <atom_type> atom
%type <atom_flag> atom_flags
%type <atom_flag> atom_flag
%type <depend_type> depend_expr
%type <use_type> required_use_expr
%type <depend_expr_select> depend_expr_sel
%type <depend_expr_select> use_expr

%%

program:                                        {yyout = NULL;}
            | depend_expr                       {yyout = (void*)$1;}
            | required_use_expr                 {yyout = (void*)$1;}
            | END_OF_FILE                       {yyout = NULL;}
            ;

required_use_expr   : depend_expr_sel '(' required_use_expr ')' {$$ = use_build_required_use($1.target, $1.t); $$->depend = $3;}
                    | required_use_expr required_use_expr       {$$ = $1; $$->next = $2;}
                    | use_expr                                  {$$ = use_build_required_use($1.target, $1.t);}
                    | '(' required_use_expr ')'                 {$$ = $2;}
                    ;

depend_expr  :    depend_expr_sel[p] depend_expr[c]         {$$ = dependency_build_use($p.target, $p.t, $c);}
                | '(' depend_expr[c] ')'                    {$$ = $c;}
                | atom                                      {$$ = dependency_build_atom($1);}
                | depend_expr depend_expr                   {$$ = $1; $1->next = $2;}
                ;

depend_expr_sel : use_expr '?'          {$$ = $1;}
                | USESELECT             {$$.target = NULL; $$.t = $1;}
                ;

use_expr: '!' IDENTIFIER        {$$.target = $2; $$.t = USE_DISABLE;}
        | IDENTIFIER            {$$.target = $1; $$.t = USE_ENABLE;}
        ;

atom        : atom_repo '[' atom_flags ']' {$$->useflags = $3;}
            | atom_repo                    {$$->useflags = NULL;}
            ;

atom_flags  : '!' atom_flag '?'     {$$ = $2; $$->option = ATOM_USE_DISABLE_IF_OFF;}
            | '!' atom_flag '='     {$$ = $2; $$->option = ATOM_USE_OPPOSITE;}
            | atom_flag '?'         {$$ = $1; $$->option = ATOM_USE_ENABLE_IF_ON;}
            | atom_flag '='         {$$ = $1; $$->option = ATOM_USE_EQUAL;}
            | atom_flag             {$$ = $1; $$->option = ATOM_USE_ENABLE;}
            | '-' atom_flag         {$$ = $2; $$->option = ATOM_USE_DISABLE;}
            | atom_flags ',' atom_flags {$$ = $1; $$->next = $3;}
            ;

atom_flag   : IDENTIFIER            {$$ = atomflag_build($1); $$->def = ATOM_NO_DEFAULT; $$->next = NULL;}
            | IDENTIFIER USE_DEFAULT{$$ = atomflag_build($1); $$->def = $2; $$->next = NULL;}
            ;

atom_repo   : atom_slot_rebuild REPOSITORY {$$ = $1; free($$->repository); $$->repository = $2;}
            | atom_slot_rebuild            {$$ = $1;}
            ;

atom_slot_rebuild:
              atom_slot '='         {$$ = $1; $$->sub_opts = ATOM_SLOT_REBUILD;}
            | atom_slot '*'         {$$ = $1; $$->sub_opts = ATOM_SLOT_IGNORE;}
            | atom_slot             {$$ = $1; $$->sub_opts = ATOM_SLOT_IGNORE;}
            ;

atom_slot   : atom_block SLOT           {
                                            $$ = $1;
                                            $$->slot = $2.name;
                                            if ($2.sub_name)
                                                $$->sub_slot = $2.sub_name;
                                        }
            | atom_block                {$$ = $1; $$->slot = NULL;}
            ;

atom_block  :      '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
            |  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |          atom_version     {$$ = $1; $$->blocks = ATOM_BLOCK_NONE;}
            ;

atom_version   : '>' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_GE;}
               | '<' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_LE;}
               |     '=' ATOM           {$$ = $2; $$->range = ATOM_VERSION_E;}
               |     '>' ATOM           {$$ = $2; $$->range = ATOM_VERSION_G;}
               |     '~' ATOM           {$$ = $2; $$->range = ATOM_VERSION_REV;}
               |     '<' ATOM           {$$ = $2; $$->range = ATOM_VERSION_L;}
               |         ATOM           {$$ = $1; $$->range = ATOM_VERSION_ALL;}
               ;
%%