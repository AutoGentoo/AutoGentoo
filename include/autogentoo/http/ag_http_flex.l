%option prefix="ag_http"

%{
#include "ag_http.tab.h"
#include <stdio.h>
#include <rfc.h>

int error = 0;

int ag_httperror(char *s) {
    fprintf(stderr, "%s in line %d near '%s'\n", s, ag_httplineno, ag_httptext);
    error = 1;
    
    return error;
}

HttpRequestLine* ag_http_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    ag_http_switch_to_buffer(bs);
    ag_httpparse();
    return (HttpRequestLine*)ag_httpout;
}

%}

letter          [a-zA-Z]
digit           [0-9]
header          ([\x21-\x39]|[\x3B-\x7E])


%%

"GET"                   {return GET;}
"HEAD"                  {return HEAD;}
"POST"                  {return POST;}

"\r\n"                  {return CRLF;}
"\t "                   {return SPHT;}
{header}+               {ag_httpval.identifier = strdup(ag_httptext);return HEADER;}
[^\r^\n]+               {ag_httpval.identifier = strdup(ag_httptext); return IDENTIFIER;}

[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", atomtext);}

%%