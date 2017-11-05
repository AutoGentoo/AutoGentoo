%option prefix="package"

%{
#include "package_bison.h"
#include <stdio.h>
#include <package.h>

int packageerror(char *s)
{
    fprintf(stderr, "%s in line %d near '%s'\n", s, packagelineno, packagetext);
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
version_section ({letter}|{digit})*
version         (({version_section}"."{version_section})|{version_section})+
identifier      {version}

%%

[ \n\t\r\\]+            ;
"/"                     {return '/';}
"-"                     {return '-';}

{identifier}            {packagelval.identifier = strdup (packagetext); return (IDENTIFIER);}

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", packagetext);}

%%