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
void* requireduseout;

#define YYERROR_VERBOSE 1

extern Portage* global_portage;

void requireduseerror(const char *message);
%}

%start program_required_use

%union {
    char* identifier;
    atom_use_default_t use_default;
    RequiredUse* required_use;
    struct {
        char* target;
        use_operator_t operator;
     } use_select;
}

%token <identifier> IDENTIFIER
%token END_OF_FILE
%token <use_default> USE_DEFAULT
%token <slot> SLOT
%token <use_select> USESELECT
%token DEPEND
%token REQUIRED_USE
%token COMMAND_LINE

%type <required_use> required_use_expr
%type <required_use> required_use_single
%type <use_select> depend_expr_sel
%type <use_select> use_expr

%destructor { free($$); } <identifier>
%destructor { if($$.target) free($$.target); } <use_select>
%destructor { Py_DECREF($$); } <required_use>

%%

program_required_use:
              REQUIRED_USE required_use_expr    {requireduseout = (void*)$2;}
            | REQUIRED_USE                      {requireduseout = NULL;}
            ;

required_use_single : use_expr                                  {$$ = use_build_required_use($1.target, $1.operator);}
                    | depend_expr_sel '(' required_use_expr ')' {
                                                                     $$ = use_build_required_use($1.target, $1.operator);
                                                                     $$->depend = $3;
                                                                }
                    | '(' required_use_expr ')'                 {$$ = $2;}
                    ;

required_use_expr   : required_use_single                         {$$ = $1;}
                    | required_use_single required_use_expr       {$$ = $1; $$->next = $2;}
                    ;

use_expr     : '!' IDENTIFIER        {$$.target = $2; $$.operator = USE_OP_DISABLE;}
             | IDENTIFIER            {$$.target = $1; $$.operator = USE_OP_ENABLE;}
             ;

depend_expr_sel : use_expr '?'       {$$ = $1;}
                | USESELECT          {$$ = $1;}
                ;

%%