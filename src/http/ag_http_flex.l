%option prefix="ag_http"

%{
#include "ag_http.tab.h"
#include <stdio.h>

int error = 0;

int ag_httperror(char *s) {
    //fprintf(stderr, "%s in line %d near '%s'\n", s, ag_httplineno, ag_httptext);
    error = 1;
    
    return error;
}

HttpRequest* ag_http_parse (char* buffer) {
    YY_BUFFER_STATE bs = yy_scan_string(buffer);
    ag_http_switch_to_buffer(bs);
    ag_httpparse();
    return (HttpRequest*)ag_httpout;
}

%}

letter          [a-zA-Z]
digit           [0-9]
header          ([\x21-\x39]|[\x3B-\x7E])
spht            (" "|\t)


%%

"GET"                   {return GET;}
"HEAD"                  {return HEAD;}
"POST"                  {return POST;}
"HTTP/"[0-9]+"."[0-9]+  {
                            sscanf(ag_httptext,
                                "HTTP/%d[^'.'].%d[^\n\r]",
                                &ag_httplval.http_version.maj,
                                &ag_httplval.http_version.min);
                            return VERSION;
                        }

"\r\n"                  {return CRLF;}
{spht}                  {return SPHT;}
[^ \r\n]+               {ag_httplval.str = strdup(ag_httptext); return STR;}

.                       {printf("Unknown character: %s\n", ag_httptext);}

%%