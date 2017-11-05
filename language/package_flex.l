%option prefix="package"

%{
#include "package_bison.h"
#include <stdio.h>
#include <package.h>
#include <share.h>

int error = 0;

int packageerror(char *s) {
    fprintf(stderr, "%s in line %d near '%s'\n", s, packagelineno, packagetext);
    error = 1;
}

PackageSelector* package_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    package_switch_to_buffer(bs);
    packageparse();
    return (PackageSelector*)packageout;
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
                            char* b = &packagetext[2];
                            sscanf(b, "%d", &packagelval.r);
                            return (REVISION);
                        }
{version}               {
                            set_package_selector_version(&packagelval.version, packagetext, 0);
                            return VERSION;
                        }

{identifier}            {
                            packagelval.identifier = strdup (packagetext);
                            return (IDENTIFIER);
                        }

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", packagetext);}

%%