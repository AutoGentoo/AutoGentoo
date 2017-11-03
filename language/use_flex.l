%{
#include "use_bison.h"
#include <stdio.h>

%}

%option noyywrap

letter  [a-zA-Z]
digit   [0-9]
atom    ({letter}|[\-\.\/])*

%%

"^^"                    {return(EXACT_ONE);}
"||"                    {return(EXACT_ONE);}
"??"                    {return(MOST_ONE);}

"!"{letter}*"?"         {return(NO_USE);}
{letter}*"?"            {return(YES_USE);}

"<="{atom}              {return(LE_ATOM);}
">="{atom}              {return(GE_ATOM);}
"="{atom}               {return(E_ATOM);}
"<"{atom}               {return(L_ATOM);}
">"{atom}               {return(G_ATOM);}
{atom}                  {return(ATOM);}

"("                     {return ('(');}
")"                     {return (')');}
[ \n\t]                 ;
.                       {printf("Unknown character: %s\n", yytext);}
<<EOF>>  return(END_OF_FILE);

%%