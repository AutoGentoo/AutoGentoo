%{
#include "use_bison.h"
#include <stdio.h>

int yyerror(char *s)
{
    fprintf(stderr, "%s in line %d near <%s>\n", s, yylineno, yytext);
}

%}

letter  [a-zA-Z]
digit   [0-9]
atom    ({letter}|{digit}|[\-\.\/_])*

%%

[ \n\t\r]+               ;

"^^"                    {yylval.use = new_use (yytext, EXACTLY_ONE); return(EXACT_ONE);}
"||"                    {yylval.use = new_use (yytext, AT_LEAST_ONE); return(LEAST_ONE);}
"??"                    {yylval.use = new_use (yytext, AT_MOST_ONE); return(MOST_ONE);}

"!"{letter}*"?"         {
                            yytext[strlen(yytext) - 1] = 0; // Get rid of '?'
                            yylval.use = new_use (&yytext[1], NOT_USE); 
                            return(NO_USE);
                        }
{letter}*"?"            {
                            yytext[strlen(yytext) - 1] = 0; // Get rid of '?'
                            yylval.use = new_use (&yytext[0], USE); 
                            return(YES_USE);
                        }

"<="{atom}              {yylval.atom = new_atom (&yytext[2], LESS_E); return(LE_ATOM);}
">="{atom}              {yylval.atom = new_atom (&yytext[2], GREAT_E); return(GE_ATOM);}
"="{atom}               {yylval.atom = new_atom (&yytext[1], EQUAL); return(E_ATOM);}
"<"{atom}               {yylval.atom = new_atom (&yytext[1], LESS); return(L_ATOM);}
">"{atom}               {yylval.atom = new_atom (&yytext[1], GREAT); return(G_ATOM);}
{atom}                  {yylval.atom = new_atom (yytext, ALL); return(ATOM);}

"("                     {return ('(');}
")"                     {return (')');}
[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", yytext);}

%%

int main() {
    char buf[] = "use? (fdf =dfg <=dfger-341 || ( df dhe ))";
    printf ("%s\n", buf);
    YY_BUFFER_STATE bs = yy_scan_string(buf);
    yy_switch_to_buffer(bs);
    yyparse();
}