%define api.prefix {depend}
%code requires {
  #include <stdlib.h>
  #include <string.h>
  #include "language.h"
}

%{
#include <stdio.h>
#include "language.h"

int dependparse(void);
int dependwrap() { return 1; }
int dependlex();
int dependlineno;
int dependlloc;
void* dependout;

#define YYERROR_VERBOSE 1

extern Portage* global_portage;

void dependerror(const char *message);
%}

%start program_depend

%union {
    char* identifier;
    Atom* atom_type;
    Dependency* depend_type;
    RequiredUse* use_type;

    struct {
        char* target;
        use_operator_t operator;
    } depend_expr_select;

    struct {
        char* name;
        char* sub_name;
        atom_slot_t slot_opts;
    } slot;

    AtomFlag* atom_flags;
}

%token <identifier> IDENTIFIER
%token <identifier> REPOSITORY
%token <atom_type> ATOM
%token END_OF_FILE
%token <atom_flags> ATOM_FLAGS
%token <slot> SLOT
%token <depend_expr_select> USESELECT
%token DEPEND
%token COMMAND_LINE

%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot
%type <atom_type> atom_repo
%type <atom_type> atom
%type <depend_type> depend_expr
%type <depend_type> depend_expr_single
//%type <depend_type> command_line
//%type <atom_type> command_atom

%destructor { if ($$.target) free($$.target); } <depend_expr_select>
%destructor { Py_DECREF($$); } <atom_type>;
%destructor { Py_DECREF($$); } <atom_flags>;
%destructor { free($$.name); if($$.sub_name) free($$.sub_name); } <slot>;
%destructor { free($$); } <identifier>;

%%

program_depend:                                 {dependout = NULL;}
            | DEPEND depend_expr                {dependout = (void*)$2;}
            //| COMMAND_LINE command_line       {dependout = $2;}
            | END_OF_FILE                       {dependout = NULL;}
            ;

depend_expr  : depend_expr_single               {$$ = $1;}
             | depend_expr depend_expr_single   {$$ = $1; $$->next = $2;}

depend_expr_single  : atom                          {$$ = dependency_build_atom($1);}
                    | USESELECT '(' depend_expr ')' {$$ = dependency_build_use($1.target, $1.operator, $3); free($1.target);}
                    | '(' depend_expr ')'           {$$ = dependency_build_grouping($2);}
                    ;

atom        : atom_repo ATOM_FLAGS          {$$ = $1; $$->useflags = $2;}
            | atom_repo                     {$$ = $1; $$->useflags = NULL;}
            ;

atom_repo   : atom_slot REPOSITORY  {
                                        $$ = $1;
                                        if ($$->repository) free($$->repository);
                                        $$->repository = $2;
                                    }
            | atom_slot             {$$ = $1;
                                     if ($$->repository) free($$->repository);
                                     $$->repository = NULL;
                                    }
            ;

atom_slot   : atom_block SLOT             {
                                               $$ = $1;
                                               $$->slot = $2.name;
                                               $$->sub_slot = $2.sub_name;
                                               $$->slot_opts = $2.slot_opts;
                                          }
            | atom_block                {$$ = $1; $$->slot = NULL; $$->sub_slot = NULL; $$->slot_opts = ATOM_SLOT_IGNORE;}
            ;

atom_block  :  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |      '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
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

/*
command_atom   : atom_repo              {$$ = $1;}
               | '>' '=' IDENTIFIER     {$$ = cmdline_atom_new($3); if ($$) $$->range = ATOM_VERSION_GE;}
               | '<' '=' IDENTIFIER     {$$ = cmdline_atom_new($3); if ($$) $$->range = ATOM_VERSION_LE;}
               |     '=' IDENTIFIER     {$$ = cmdline_atom_new($2); if ($$) $$->range = ATOM_VERSION_E;}
               |     '>' IDENTIFIER     {$$ = cmdline_atom_new($2); if ($$) $$->range = ATOM_VERSION_G;}
               |     '~' IDENTIFIER     {$$ = cmdline_atom_new($2); if ($$) $$->range = ATOM_VERSION_REV;}
               |     '<' IDENTIFIER     {$$ = cmdline_atom_new($2); if ($$) $$->range = ATOM_VERSION_L;}
               |         IDENTIFIER     {$$ = cmdline_atom_new($1); if ($$) $$->range = ATOM_VERSION_ALL;}
               ;

command_line   : command_atom ATOM_FLAGS         {$1->useflags = $2; $$ = dependency_build_atom($1);}
               | command_atom                    {$$ = dependency_build_atom($1);}
               | command_line command_line       {$$ = $1; $$->next = $2;}
               ;
*/
%%