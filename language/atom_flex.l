%option prefix="atom"

%{
#include "atom.tab.h"
#include <stdio.h>
#include <atom.h>
#include <share.h>

int error = 0;

int atomerror(char *s) {
    fprintf(stderr, "%s in line %d near '%s'\n", s, atomlineno, atomtext);
    error = 1;
    
    return error;
}

AtomSelector* atom_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    atom_switch_to_buffer(bs);
    atomparse();
    return (AtomSelector*)atomout;
}

%}

letter          [a-zA-Z]
digit           [0-9]
version_suf     ("_alpha"|"_beta"|"_pre"|"_rc"|""|"_p")
version         {digit}+("."{digit}+)*{letter}?({version_suf}{digit}*)*
identifier      ({letter}|{digit}|"_"|"+")+

%%

[ \n\t\r\\]+            ;
"/"                     {return '/';}
"-"                     {return '-';}
"-r"{digit}+            {
                            char* b = &atomtext[2];
                            sscanf(b, "%d", &atomlval.r);
                            return (REVISION);
                        }
{version}               {
                            set_atom_selector_version(&atomlval.version, atomtext, 0);
                            return VERSION;
                        }

{identifier}            {
                            atomlval.identifier = strdup (atomtext);
                            return (IDENTIFIER);
                        }

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", atomtext);}

%%