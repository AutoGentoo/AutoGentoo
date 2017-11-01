%{
#include <stdio.h>

void yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
}

int sym[26];
%}

%union {
  int number;
  int var;
};

%type <number> expression
%token <var> VAR
%token <number> NUMBER

%%

start: expression '\n' { printf("%d\n\n", $1); } start
     | /* NULL */
     ;

expression: NUMBER
          | VAR                       { $$ = sym[$1]; }
          | '-' expression            { $$ = -$2; }
          | expression '+' expression { $$ = $1 + $3; }
          | expression '-' expression { $$ = $1 - $3; }
          | expression '*' expression { $$ = $1 * $3; }
          | '(' expression ')'        { $$ = $2; }
          | VAR '=' expression        { sym[$1] = $3; $$ = $3; }
          ;