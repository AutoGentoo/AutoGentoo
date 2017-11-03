%{
#include <stdio.h>

int yyparse(void);
int yylex(void);
int yywrap() { return 1; }
extern int yylineno;
extern char* yytext;
extern int yylval;

/* #define YYSTYPE char * */

void yyerror(const char *message) {
  fprintf(stderr, "%d: error: '%s' at '%s', yylval=%u\n", yylineno, message, yytext, yylval);
}

int sym[26];
%}

%start program

%token NO_USE YES_USE
%token EXACT_ONE LEAST_ONE MOST_ONE
%token ATOM LE_ATOM GE_ATOM L_ATOM G_ATOM E_ATOM
%token END_OF_FILE
%token L_PAREN R_PAREN

%%

program:        /* empty */
                | expr {}
                | END_OF_FILE {printf("Encountered EOF\n");}
                ;

expr : use L_PAREN expr R_PAREN
     | select expr
     | expr select
     | select
     ;

use : NO_USE
    | YES_USE
    | EXACT_ONE
    | LEAST_ONE
    | MOST_ONE
    ;

select  : ATOM
        | LE_ATOM
        | GE_ATOM
        | L_ATOM
        | G_ATOM
        | E_ATOM
        ;