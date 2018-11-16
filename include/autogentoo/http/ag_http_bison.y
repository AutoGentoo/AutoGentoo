%define api.prefix {ag_http}

%code requires {
  #include <stdlib.h>
  #include <string.h>
  #include "http.h"
}

%{
#include <stdio.h>
#include <http.h>

int ag_httpparse(void);
int ag_httpwrap() { return 1; }
int ag_httplex();
extern int ag_httplineno;
extern char* ag_httptext;
extern HttpRequestLine* ag_httpout;

void ag_httperror(const char *message);
%}

%start request

%union {
    char* identifier;
    struct {
        int maj;
        int min;
    } http_version;

    struct {
        char* path;
        http_version version;
    } requestline;

    HttpHeader* header;
    HttpRequest* request;
    SmallMap* smallmap;
    http_function_t function_type;
}

%token <identifier> HEADER;
%token http_version VERSION;

%token GET
%token HEAD
%token POST

%token SPHT
%token CRLF;

%token <identifier> IDENTIFIER
%token END_OF_FILE

%token <requestline> requestline
%token <function_type> request_function
%token <header> header
%token <smallmap> headers
%token <identifier> body

%%

requestline: SPHT IDENTIFIER[p] SPHT VERSION[v] CRLF {$$.version = $v; $$.path = $p;};
request:      request_function requestline headers {
                                                $$ = malloc(sizeof (HTTPRequest))
                                                $$->function = $1;
                                                $$->path = $2.path;
                                                $$->version = $2.version;
                                                $$->headers = $3;
                                                $$->body_start = @3.last_column;
                                            }
            ;

request_function:   GET                     {$$ = HTTP_FUNCTION_GET}
                    | HEAD                  {$$ = HTTP_FUNCTION_HEAD}
                    | POST                  {$$ = HTTP_FUNCTION_POST}

headers:      header                        {
                                                $$ = small_map_new (5, 5);
                                                vector_append($$, &$1);
                                                @$.last_column = @1.last_column;
                                            }
            | header headers                {
                                                vector_append($2, &$1);
                                                $$ = $2;
                                                @$.last_column = @1.last_column;
                                            }

header: HEADER[h] ':' ' ' IDENTIFIER[i] CRLF{
                                                $$ = malloc(sizeof (HTTPRequest));
                                                $$.name = $h;
                                                $$.value = $i;
                                                @$.last_column = @5.last_column;
                                            }

%%