%define api.prefix {depend}

%code requires {
  #include "share.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
#include <stdio.h>
#include "share.h"

int dependparse(void);
int dependwrap() { return 1; }
int dependlex();
extern int dependlineno;
extern char* dependtext;
extern Dependency* dependout;

void dependerror(const char *message);
%}

%start program

%union {
    char* identifier;
    P_Atom* atom_type;
    Dependency* depend_type;
    atom_use_default use_default;

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
%token <atom_type> ATOM
%token END_OF_FILE
%token <use_default> USE_DEFAULT;
%token <slot> SLOT

%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot_rebuild
%type <atom_type> atom_slot
%type <atom_type> atom
%type <atom_flag> atom_flags
%type <atom_flag> atom_flag
%type <depend_type> depend_expr
%type <depend_expr_select> depend_expr_sel

%%

program:    | depend_expr                      {dependout = $1;}
            | END_OF_FILE
            ;

depend_expr  :    depend_expr_sel[p] '(' depend_expr[c] ')' {$$ = dependency_build_use($p.target, $p.t, $c);}
                | '(' depend_expr[c] ')'                    {$$ = $c;}
                | atom                                      {$$ = dependency_build_atom($1);}
                | depend_expr depend_expr                   {$$ = $1; $1->next = $2;}
                ;

depend_expr_sel : '!' IDENTIFIER '?'    {$$.target = $2; $$.t = USE_DISABLE;}
                | IDENTIFIER '?'        {$$.target = $1; $$.t = USE_ENABLE;}
                | '|' '|'               {$$.target = "||"; $$.t = USE_LEAST_ONE;}
                | '^' '^'               {$$.target = "^^"; $$.t = USE_EXACT_ONE;}
                | '?' '?'               {$$.target = "??"; $$.t = USE_MOST_ONE;}
                ;

atom        : atom_slot_rebuild '[' atom_flags ']' {$$->useflags = $3;}
            | atom_slot_rebuild
            ;

atom_flags  : '!' atom_flag '?'     {$$ = $2; $$->option = ATOM_USE_DISABLE_IF_OFF;}
            | '!' atom_flag '='     {$$ = $2; $$->option = ATOM_USE_OPPOSITE;}
            | atom_flag '?'         {$$ = $1; $$->option = ATOM_USE_ENABLE_IF_ON;}
            | atom_flag '='         {$$ = $1; $$->option = ATOM_USE_EQUAL;}
            | atom_flag             {$$ = $1; $$->option = ATOM_USE_ENABLE;}
            | '-' atom_flag         {$$ = $2; $$->option = ATOM_USE_DISABLE;}
            | atom_flags ',' atom_flags {$$ = $1; $$->next = $3;}
            ;

atom_flag   : IDENTIFIER            {$$ = atomflag_build($1); $$->def = ATOM_NO_DEFAULT;}
            | IDENTIFIER USE_DEFAULT{$$ = atomflag_build($1); $$->def = $2;}

atom_slot_rebuild:
              atom_slot '='         {$$ = $1; $$->sub_opts = ATOM_SLOT_REBUILD;}
            | atom_slot '*'         {$$ = $1; $$->sub_opts = ATOM_SLOT_IGNORE;}
            | atom_slot             {$$ = $1; $$->sub_opts = ATOM_SLOT_IGNORE;}

atom_slot   : atom_block SLOT           {
                                            $$ = $1;
                                            $$->slot = $2.name;
                                            if ($2.sub_name)
                                                $$->sub_slot = $2.sub_name;
                                        }
            | atom_block                {$$ = $1; $$->slot = NULL;}

atom_block  :      '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
            |  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |          atom_version     {$$ = $1; $$->blocks = ATOM_BLOCK_NONE;}
            ;

atom_version   :     '=' ATOM           {$$ = $2; $$->range = ATOM_VERSION_E;}
               |     '>' ATOM           {$$ = $2; $$->range = ATOM_VERSION_G;}
               |     '~' ATOM           {$$ = $2; $$->range = ATOM_VERSION_REV;}
               |     '<' ATOM           {$$ = $2; $$->range = ATOM_VERSION_L;}
               | '>' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_GE;}
               | '<' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_LE;}
               |         ATOM           {$$ = $1; $$->range = ATOM_VERSION_ALL;}
               ;
%%