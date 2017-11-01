%{
#include "use_bison.h"
%}

%option noyywrap
%option always-interactive

%%

[0-9]+     { yylval.number = atoi(yytext); return NUMBER; }

[a-z][a-zA-Z0-9]+      { yylval.var = *yytext - 'a'; return VAR; }

[-+()*\n=] { return *yytext; }

[ \t]      ;

.          yyerror("Unknown character");

%%