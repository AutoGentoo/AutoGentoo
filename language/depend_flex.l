%option prefix="depend"

%{
#include "depend_bison.h"
#include <stdio.h>

int dependerror(char *s)
{
    fprintf(stderr, "%s in line %d near <%s>\n", s, dependlineno, dependtext);
}

%}

letter  [a-zA-Z]
digit   [0-9]
atom    ({letter}|{digit}|[\-\.\/_])*

%%

[ \n\t\r\\]+             ;

"^^"                    {dependlval.use = new_use (dependtext, EXACTLY_ONE); return(EXACT_ONE);}
"||"                    {dependlval.use = new_use (dependtext, AT_LEAST_ONE); return(LEAST_ONE);}
"??"                    {dependlval.use = new_use (dependtext, AT_MOST_ONE); return(MOST_ONE);}

"!"{letter}*"?"         {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (&dependtext[1], NOT_USE); 
                            return(NO_USE);
                        }
{letter}*"?"            {
                            dependtext[strlen(dependtext) - 1] = 0; // Get rid of '?'
                            dependlval.use = new_use (&dependtext[0], USE); 
                            return(YES_USE);
                        }

"<="{atom}              {dependlval.atom = new_atom (&dependtext[2], LESS_E); return(LE_ATOM);}
">="{atom}              {dependlval.atom = new_atom (&dependtext[2], GREAT_E); return(GE_ATOM);}
"="{atom}               {dependlval.atom = new_atom (&dependtext[1], EQUAL); return(E_ATOM);}
"<"{atom}               {dependlval.atom = new_atom (&dependtext[1], LESS); return(L_ATOM);}
">"{atom}               {dependlval.atom = new_atom (&dependtext[1], GREAT); return(G_ATOM);}
{atom}                  {dependlval.atom = new_atom (dependtext, ALL); return(ATOM);}

"("                     {return ('(');}
")"                     {return (')');}
[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", dependtext);}

%%

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf (stderr, "Enter a string to parse!\n");
        return 1;
    }
    printf ("%s\n", argv[1]);
    YY_BUFFER_STATE bs = yy_scan_string(argv[1]);
    yy_switch_to_buffer(bs);
    dependparse();
}