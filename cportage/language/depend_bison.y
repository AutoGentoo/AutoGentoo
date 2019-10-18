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
    struct {
        use_select_t val;
        char* target;
     } use_select;

    struct {
        char* target;
        use_select_t t;
    } depend_expr_select;

    struct {
        char* name;
        char* sub_name;
        atom_slot_t sub_opts;
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
%token DEPEND
%token REQUIRED_USE
%token COMMAND_LINE

%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot
%type <atom_type> atom_repo
%type <atom_type> atom
%type <atom_flag> atom_flags
%type <atom_flag> atom_flag
%type <depend_type> depend_expr
%type <use_type> required_use_expr
%type <depend_expr_select> depend_expr_sel
%type <depend_expr_select> use_expr
%type <depend_type> command_line
%type <atom_type> command_atom

%%

program:                                        {yyout = NULL;}
            | DEPEND depend_expr                {yyout = (void*)$2;}
            | REQUIRED_USE required_use_expr    {yyout = (void*)$2;}
            | COMMAND_LINE command_line         {yyout = $2;}
            | END_OF_FILE                       {yyout = NULL;}
            ;

required_use_expr   : depend_expr_sel '(' required_use_expr ')' {$$ = use_build_required_use($1.target, $1.t); free($1.target); $$->depend = $3;}
                    | required_use_expr required_use_expr       {$$ = $1; $$->next = $2;}
                    | use_expr                                  {$$ = use_build_required_use($1.target, $1.t);}
                    | '(' required_use_expr ')'                 {$$ = $2;}
                    ;

depend_expr  :    depend_expr_sel[p] '(' depend_expr[c] ')' {$$ = dependency_build_use($p.target, $p.t, $c); free($p.target);}
                | '(' depend_expr[c] ')'                    {$$ = $c;}
                | atom                                      {$$ = dependency_build_atom($1);}
                | depend_expr depend_expr                   {$$ = $1; $1->next = $2;}
                ;

depend_expr_sel : use_expr '?'          {$$ = $1;}
                | USESELECT             {$$.target = $1.target; $$.t = $1.val;}
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
            | atom_flag             {$$ = $1;}
            | atom_flags ',' atom_flags {$$ = $1; $$->next = $3;}
            ;

atom_flag   : IDENTIFIER            {$$ = atomflag_build($1); free($1); $$->def = ATOM_NO_DEFAULT; $$->next = NULL;}
            | IDENTIFIER USE_DEFAULT{$$ = atomflag_build($1); free($1); $$->def = $2; $$->next = NULL;}
            ;

atom_repo   : atom_slot REPOSITORY {$$ = $1; free($$->repository); $$->repository = $2; $$->repo_selected = ATOM_REPO_DEFINED;}
            | atom_slot            {$$ = $1; $$->repo_selected = ATOM_REPO_ALL;}
            ;

atom_slot   : atom_block                {$$ = $1; $$->slot = NULL; $$->sub_slot = NULL; $$->sub_opts = ATOM_SLOT_IGNORE;}
            | atom_block SLOT           {
                                             $$ = $1;
                                             $$->slot = $2.name;
                                             $$->sub_slot = $2.sub_name;
                                             $$->sub_opts = $2.sub_opts;
                                         }
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

command_atom   : atom_repo              {$$ = $1;}
               | '>' '=' IDENTIFIER     {$$ = cmdline_atom_new($3); $$->range = ATOM_VERSION_GE;}
               | '<' '=' IDENTIFIER     {$$ = cmdline_atom_new($3); $$->range = ATOM_VERSION_LE;}
               |     '=' IDENTIFIER     {$$ = cmdline_atom_new($2); $$->range = ATOM_VERSION_E;}
               |     '>' IDENTIFIER     {$$ = cmdline_atom_new($2); $$->range = ATOM_VERSION_G;}
               |     '~' IDENTIFIER     {$$ = cmdline_atom_new($2); $$->range = ATOM_VERSION_REV;}
               |     '<' IDENTIFIER     {$$ = cmdline_atom_new($2); $$->range = ATOM_VERSION_L;}
               |         IDENTIFIER     {$$ = cmdline_atom_new($1); $$->range = ATOM_VERSION_ALL;}
               ;

command_line   : command_atom                    {$$ = dependency_build_atom($1);}
               | command_atom '[' atom_flags ']' {$1->useflags = $3; $$ = dependency_build_atom($1);}
               | command_line command_line       {$$ = $1; $$->next = $2;}
               ;

%%