%define api.prefix {ag_http}

%code requires {
    #define _GNU_SOURCE
    #include <stdlib.h>
    #include <string.h>
    #include "autogentoo/http.h"
    #include "autogentoo/hacksaw/tools.h"
}

%{
#define _GNU_SOURCE
#include <stdio.h>
#include "autogentoo/http.h"
#include "autogentoo/hacksaw/tools.h"

int ag_httpparse(void);
int ag_httpwrap() { return 1; }
int ag_httplex();
extern int ag_httplineno;
extern char* ag_httptext;
extern HttpRequest* ag_httpout;

void ag_httperror(const char *message);
%}

%start request

%union {
    char* str;
    struct __http_version {
        int maj;
        int min;
    } http_version;

    HttpHeader* header;
    HttpRequest* request;
    SmallMap* smallmap;
    request_t function_type;
}

%token <header> HEADER;
%token <http_version> VERSION;

%token SPHT;
%token CRLF;

%token GET
%token HEAD;
%token POST;

%token <str> STR;
%token <str> HEADER_STR;
%token END_OF_FILE;

%type <function_type> request_function;
%type <smallmap> headers;
%type <str> end;

%%
request:      request_function[f] SPHT STR[p] SPHT VERSION[v] CRLF headers[h] end[e] {
                                                ag_httpout = malloc(sizeof (HttpRequest));
                                                ag_httpout->function = $f;
                                                ag_httpout->path = $p;
                                                ag_httpout->version.maj = $v.maj;
                                                ag_httpout->version.min = $v.min;
                                                ag_httpout->headers = $h;
                                                ag_httpout->body_start = @e.last_column;
                                            }
            ;

end:                                        {$$ = NULL;}
            | CRLF                          {@$.last_column = @1.last_column;};

request_function:   GET                     {$$ = REQ_GET;}
                    | HEAD                  {$$ = REQ_HEAD;}
                    | POST                  {$$ = REQ_POST;}
                    ;

headers:                                    {$$ = small_map_new (5, 5);}
            | HEADER                        {
                                                $$ = small_map_new (5, 5);
                                                small_map_insert($$, $1->name, $1->value);
                                                free($1);
                                                @$.last_column = @1.last_column;
                                            }
            | headers HEADER                {
                                                $$ = $1;
                                                small_map_insert($$, $2->name, $2->value);
                                                @$.last_column = @2.last_column;
                                            }
            ;
%%