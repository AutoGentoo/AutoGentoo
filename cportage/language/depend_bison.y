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
    AtomVersion* version;
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
%token <version> VERSION
%token END_OF_FILE
%token <use_default> USE_DEFAULT;
%token <slot> SLOT

%type <atom_type> atom_simple
%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot
%type <atom_type> atom
%type <atom_flag> atom_flags
%type <atom_flag> atom_flag
%type <depend_type> depend_expr
%type <depend_expr_select> depend_expr_sel
%type <identifier> ident

%%

program:    | depend_expr                      {dependout = $1;}
            | END_OF_FILE
            ;

depend_expr  :
                  depend_expr_sel[p] '(' depend_expr[c] ')' {$$ = dependency_build_use($p.target, $p.t, $c);}
                | atom                                      {$$ = dependency_build_atom($1);}
                | depend_expr depend_expr                   {$$ = $1; $1->next = $2;}
                ;

depend_expr_sel : '!' ident '?'     {$$.target = $2; $$.t = USE_DISABLE;}
                | ident '?'         {$$.target = $1; $$.t = USE_ENABLE;}
                | '|' '|'           {$$.target = "||"; $$.t = USE_LEAST_ONE;}
                | '^' '^'           {$$.target = "^^"; $$.t = USE_EXACT_ONE;}
                | '?' '?'           {$$.target = "??"; $$.t = USE_MOST_ONE;}
                ;

atom        : atom_slot '[' atom_flags ']' {$$->useflags = $3;}
            | atom_slot
            ;

atom_flags  : '!' atom_flag '?'     {$$ = $2; $$->option = ATOM_USE_DISABLE_IF_OFF;}
            | '!' atom_flag '='     {$$ = $2; $$->option = ATOM_USE_OPPOSITE;}
            | atom_flag '?'         {$$ = $1; $$->option = ATOM_USE_ENABLE_IF_ON;}
            | atom_flag '='         {$$ = $1; $$->option = ATOM_USE_EQUAL;}
            | atom_flag             {$$ = $1; $$->option = ATOM_USE_ENABLE;}
            | '-' atom_flag         {$$ = $2; $$->option = ATOM_USE_DISABLE;}
            | atom_flags ',' atom_flags {$$ = $1; $$->next = $3;}
            ;

atom_flag   : ident                 {$$ = atomflag_build($1); $$->def = ATOM_NO_DEFAULT;}
            | ident USE_DEFAULT     {$$ = atomflag_build($1); $$->def = $2;}

atom_slot   : atom_block SLOT           {
                                            $$ = $1;
                                            $$->slot = strdup($2.name);
                                            if ($2.sub_name)
                                                $$->sub_slot = strdup($2.sub_name);
                                             $$->sub_opts = ATOM_SLOT_IGNORE;
                                        }
            | atom_block SLOT '='       {
                                             $$ = $1;
                                             $$->slot = strdup($2.name);
                                             if ($2.sub_name)
                                                 $$->sub_slot = strdup($2.sub_name);
                                             $$->sub_opts = ATOM_SLOT_REBUILD;
                                        }
            | atom_block SLOT '*'       {
                                             $$ = $1;
                                             $$->slot = strdup($2.name);
                                             if ($2.sub_name)
                                                 $$->sub_slot = strdup($2.sub_name);
                                             $$->sub_opts = ATOM_SLOT_IGNORE;
                                        }
            | atom_block

atom_block  :      '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
            |  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |          atom_version     {$$ = $1; $$->blocks = ATOM_BLOCK_NONE;}
            ;

atom_version :       '=' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_E;}
               |     '>' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_G;}
               |     '<' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_L;}
               | '>' '=' atom_simple       {$$ = $3; $$->range = ATOM_VERSION_GE;}
               | '<' '=' atom_simple       {$$ = $3; $$->range = ATOM_VERSION_LE;}
               |         atom_simple       {$$ = $1; $$->range = ATOM_VERSION_ALL;}
               ;

atom_simple :      ident[cat] '/' ident[name] '-' VERSION[version] {
                                            $$ = atom_new ($cat, $name);
                                            $$->version = $version;
                                        }
          | ident[cat] '/' ident[name] {$$ = atom_new ($cat, $name); $$->version = NULL;}
        ;
            
ident :  ident '-' IDENTIFIER           {
                                            $$ = $1;
                                            strcat ($$, "-");
                                            strcat ($$, $3);
                                        }
            | IDENTIFIER                {
                                            $$ = malloc (256);
                                            $$[0] = 0;
                                            strcat ($$, $1);
                                            free ($1);
                                        }
            ;

%%