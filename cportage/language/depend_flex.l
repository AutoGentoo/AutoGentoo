%option prefix="depend"

%{
#define _GNU_SOURCE
#include "depend.tab.h"
#include <stdio.h>
#include <share.h>
#include <string.h>

int error = 0;

int dependerror(char *s) {
    fprintf(stderr, "%s in line %d near '%s'\n", s, dependlineno, dependtext);
    error = 1;
    
    return error;
}

Dependency* depend_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    depend_switch_to_buffer(bs);
    dependparse();
    return (Dependency*)dependout;
}

%}

letter          [a-zA-Z]
digit           [0-9]
version_suf     ("_alpha"|"_beta"|"_pre"|"_rc"|""|"_p")
version         {digit}+("."{digit}+)*{letter}?({version_suf}{digit}*)*("-r"{digit}+)?
identifier      ({letter}|{digit}|"_"|"+"|"*")+
slotname        ({letter}|{digit}|"_")({letter}|{digit}|"_"|"-"|"."|"+")*
slot            \:{slotname}(\/{slotname})?

%%

[ \n\t\r\\]+            ;
{slot}                  {
                            char* name_splt = strchr(dependtext + 1, '/');
                            if (name_splt)
                                *name_splt = 0;
                            dependlval.slot.name = strdup(dependtext + 1);
                            if (name_splt)
                                dependlval.slot.sub_name = strdup(name_splt + 1);
                            else
                                dependlval.slot.sub_name = NULL;
                            return SLOT;
                        }
[/]                     {return '/';}
[-]                     {return '-';}
[<]                     {return '<';}
[>]                     {return '>';}
[=]                     {return '=';}
[!]                     {return '!';}
[\[]                    {return '[';}
[\]]                    {return ']';}
[\?]                    {return '?';}
[\(]                    {return '(';}
[\)]                    {return ')';}
[\^]                    {return '^';}
[\|]                    {return '|';}
[\,]                    {return ',';}
"(+)"                   {dependlval.use_default = ATOM_DEFAULT_ON; return USE_DEFAULT;}
"(-)"                   {dependlval.use_default = ATOM_DEFAULT_OFF; return USE_DEFAULT;}
{version}               {
                            dependlval.version = atom_version_new(dependtext);
                            return VERSION;
                        }

{identifier}            {
                            dependlval.identifier = strdup (dependtext);
                            return (IDENTIFIER);
                        }


[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", dependtext);}

%%