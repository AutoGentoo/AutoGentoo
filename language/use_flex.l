%{
#include "use_bison.h"
#include <stdio.h>
%}

%option noyywrap

%%

["!"][\w]*[\?]          {return(NO_USE);}
[[\w]*[\?]]               {printf("yes %s\n", yytext); return(YES_USE);}

"<="[\w\-.\/]+          {return(LE_ATOM);}
">="([\w\-\.\/]+)       {return(GE_ATOM);}
"="([\w\-\.\/]+)        {return(E_ATOM);}
"<"([\w\-\.\/]+)        {return(L_ATOM);}
">"[\S]+                {return(G_ATOM);}
[\S]+                   {return(ATOM);}

"^^"                    {return(EXACT_ONE);}
"||"                    {return(EXACT_ONE);}
"??"                    {return(MOST_ONE);}

[\(\)]                ;
[ \n\t]               ;

.                       {printf("Unknown character: %s\n", yytext);}

%%