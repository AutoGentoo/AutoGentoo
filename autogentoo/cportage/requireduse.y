%top {
#include <stdlib.h>
#include <string.h>
#include "language.h"
}

// This is default, just want to test the parser
%option parser_type="LALR(1)"
//%option disable_locks="TRUE"
%option debug_table="TRUE"
//%option debug_ids="$d+-/*^()ES"
%option prefix="required_use"

%start<required_use> program_required_use
%type <required_use> program_required_use

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
%token <use_default> USE_DEFAULT
%token <slot> SLOT
%token <use_select> USESELECT

%type <required_use> required_use_expr
%type <required_use> required_use_single
%type <use_select> depend_expr_sel
%type <use_select> use_expr

%token '-'
%token '<'
%token '>'
%token '='
%token '!'
%token '['
%token ']'
%token '?'
%token '('
%token ')'
%token '^'
%token '|'
%token ','
%token '~'

//%destructor { free($$); } <identifier>
//%destructor { if($$.target) free($$.target); } <use_select>
//%destructor { Py_DECREF($$); } <required_use>


/*
TODO
+letter          "[a-zA-Z]"
+digit           "[0-9]"
+identifier      "({letter}|{digit}|_|\+|\*|\-)+"
*/

==

"[\n]"                  {}
"[ \t\r\\]+"            {/* skip */}
"\\?\\?"                    {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_MOST_ONE; return USESELECT;}
"\\|\\|"                    {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_LEAST_ONE; return USESELECT;}
"\\^\\^"                    {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_EXACT_ONE; return USESELECT;}
"\\(\\+\\)"                   {yyval->use_default = ATOM_DEFAULT_ON; return USE_DEFAULT;}
"\\(\\-\\)"                   {yyval->use_default = ATOM_DEFAULT_OFF; return USE_DEFAULT;}
"-"                     {return '-';}
"<"                     {return '<';}
">"                     {return '>';}
"="                     {return '=';}
"!"                     {return '!';}
"[\\[]"                 {return '[';}
"[\\]]"                 {return ']';}
"[\\?]"                 {return '?';}
"[\\(]"                 {return '(';}
"[\\)]"                 {return ')';}
"[\\^]"                 {return '^';}
"[\\|]"                 {return '|';}
"[\\,]"                 {return ',';}
"[\\~]"                 {return '~';}
"{identifier}"          {
                            yyval->identifier = strdup(yytext);
                            return IDENTIFIER;
                        }

==

%%

program_required_use: required_use_expr   {$$ = (void*)$1;}
            |                             {$$ = NULL;}
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