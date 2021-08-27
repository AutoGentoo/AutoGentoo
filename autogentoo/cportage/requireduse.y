%include {
    #include <autogentoo/cportage/dependency.h>
    #include <autogentoo/cportage/use.h>
}

%top {
#include <assert.h>
}

// This is default, just want to test the parser
%option parser_type="LALR(1)"
%option annotate_line="FALSE"
%option debug_table="TRUE"
%option prefix="required_use"

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
%token <use_select> USESELECT

%type <required_use> required_use_expr
%type <required_use> required_use_single
%type <use_select> depend_expr_sel
%type <use_select> use_expr

%start<required_use> program_required_use
%type <required_use> program_required_use

%token '!'
%token '?'
%token '('
%token ')'

%option debug_ids="$ids!?()ESDRP"

%destructor <identifier> { free($$); }
%destructor <use_select> { if($$.target) free($$.target); } 
%destructor <required_use> { Py_DECREF($$); }


+identifier      [A-Za-z_0-9][A-Za-z_0-9\-\+]*

==

"[\n]"                  {}
"[ \t\r\\]+"            {/* skip */}
"\?\?"                  {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_MOST_ONE; return USESELECT;}
"\|\|"                  {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_LEAST_ONE; return USESELECT;}
"\^\^"                  {yyval->use_select.target = NULL; yyval->use_select.operator = USE_OP_EXACT_ONE; return USESELECT;}
"\(\+\)"                {yyval->use_default = ATOM_DEFAULT_ON; return USE_DEFAULT;}
"\(\-\)"                {yyval->use_default = ATOM_DEFAULT_OFF; return USE_DEFAULT;}
"!"                     {return '!';}
"\?"                    {return '?';}
"\("                    {return '(';}
"\)"                    {return ')';}
"{identifier}"          {
                            yyval->identifier = strdup(yytext);
                            return IDENTIFIER;
                        }

==

%%

program_required_use: required_use_expr   {$$ = $1;}
            |                             {$$ = NULL;}
            ;

required_use_expr   : required_use_single                         {$$ = $1; assert($$);}
                    | required_use_single required_use_expr       {$$ = $1; $$->next = $2;}
                    ;

required_use_single : use_expr                                    {$$ = use_build_required_use($1.target, $1.operator);}
                    | depend_expr_sel '(' required_use_expr ')'   {
                                                                      $$ = use_build_required_use($1.target, $1.operator);
                                                                      $$->depend = $3;
                                                                  }
                    | '(' required_use_expr ')'                   {$$ = $2; assert((U64)$2 > 10);}
                    ;

depend_expr_sel : use_expr '?'       {$$ = $1;}
                | USESELECT          {$$ = $1;}
                ;

use_expr     : '!' IDENTIFIER        {$$.target = $2; $$.operator = USE_OP_DISABLE;}
             | IDENTIFIER            {$$.target = $1; $$.operator = USE_OP_ENABLE;}
             ;

%%
