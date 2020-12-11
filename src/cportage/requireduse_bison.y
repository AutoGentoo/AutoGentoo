%define api.prefix {requireduse}
%code requires {
  #include <stdlib.h>
  #include <string.h>
  #include "language.h"
}

%{
#include <stdio.h>
#include "language.h"

int requireduseparse(void);
int requiredusewrap() { return 1; }
int requireduselex();
int requireduselineno;
int requireduselloc;
char* yytext;
void* requireduseout;

#define REQUIREDUSEERROR_VERBOSE 1

extern Portage* global_portage;

void requireduseerror(const char *message);
%}

%start program

%union {
    char* identifier;
    Atom* atom_type;
    Dependency* depend_type;
    atom_use_default_t use_default;
    RequiredUse* use_type;
    struct {
        use_operator_t val;
        char* target;
     } use_select;

    struct {
        char* target;
        use_operator_t t;
    } depend_expr_select;

    struct {
        char* name;
        char* sub_name;
        atom_slot_t slot_opts;
    } slot;

    AtomFlag* atom_flag;
}

%token <identifier> IDENTIFIER
%token END_OF_FILE
%token <use_default> USE_DEFAULT
%token <slot> SLOT
%token <use_select> USESELECT
%token DEPEND
%token REQUIRED_USE
%token COMMAND_LINE

%type <use_type> required_use_expr
%type <depend_expr_select> depend_expr_sel
%type <depend_expr_select> use_expr

%%

program:                                        {requireduseout = NULL;}
            | REQUIRED_USE required_use_expr    {requireduseout = (void*)$2;}
            | END_OF_FILE                       {requireduseout = NULL;}
            ;

required_use_expr   : depend_expr_sel required_use_expr         {
                                                                     $$ = use_build_required_use($1.target, $1.t);
                                                                     free($1.target);
                                                                     $$->depend = $2;
                                                                }
                    | required_use_expr required_use_expr       {$$ = $1; $$->next = $2;}
                    | '(' required_use_expr ')'                 {$$ = $2;}
                    | use_expr                                  {$$ = use_build_required_use($1.target, $1.t); free($1.target);}
                    ;

use_expr     : '!' IDENTIFIER        {$$.target = $2; $$.t = USE_OP_DISABLE;}
             | IDENTIFIER            {$$.target = $1; $$.t = USE_OP_ENABLE;}
             ;

depend_expr_sel : use_expr '?'       {$$ = $1;}
                | USESELECT          {$$.target = $1.target; $$.t = $1.val;}
                ;

%%