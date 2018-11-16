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

    struct {
        char* path;
        struct __http_version version;
    } request_line;

    HttpHeader* header;
    HttpRequest* request;
    SmallMap* smallmap;
    request_t function_type;
}

%token <str> HEADER;
%token <http_version> VERSION;

%token SPHT;
%token CRLF;

%token GET
%token HEAD;
%token POST;

%token <str> STR;
%token END_OF_FILE;

%type <request_line> requestline;
%type <function_type> request_function;
%type <header> header;
%type <smallmap> headers;
%type <str> text;
%token <str> body;

%%

requestline: SPHT STR[p] SPHT VERSION[v] CRLF{$$.version = $v; $$.path = $p;};
request:      request_function requestline headers {
                                                ag_httpout = malloc(sizeof (HttpRequest));
                                                ag_httpout->function = $1;
                                                ag_httpout->path = $2.path;
                                                ag_httpout->version.maj = $2.version.maj;
                                                ag_httpout->version.min = $2.version.min;
                                                ag_httpout->headers = $3;
                                                ag_httpout->body_start = @3.last_column;
                                            }
            ;

request_function:   GET                     {$$ = REQ_GET;}
                    | HEAD                  {$$ = REQ_HEAD;}
                    | POST                  {$$ = REQ_POST;}

headers:      header                        {
                                                $$ = small_map_new (5, 5);
                                                vector_add($$, &$1);
                                                @$.last_column = @1.last_column;
                                            }
            | header headers                {
                                                vector_add($2, &$1);
                                                $$ = $2;
                                                @$.last_column = @1.last_column;
                                            }

header: STR[h] SPHT text[i] CRLF[e]         {
                                                $$ = malloc(sizeof (HttpHeader));
                                                $$->name = $h;
                                                $$->name[strlen($h) - 1] = 0; // Delete ':'
                                                $$->value = $i;
                                                @$.last_column = @e.last_column;
                                            }

text: SPHT                                  {$$ = strdup (" ");}
    | STR                                   {$$ = $1;}
    | text text                             {
                                                asprintf(&$$, "%s%s", $1, $2);
                                                free($1);
                                                free($2);
                                            }
    ;

%%