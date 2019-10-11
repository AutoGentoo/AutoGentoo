%define api.prefix {mc}
// Emitted in the header file, after the definition of YYSTYPE.
%code provides
{
  // Tell Flex the expected prototype of yylex.
  #define YY_DECL                             \
    int mclex (void)

  // Declare the scanner.
  YY_DECL;
}

%code requires {
  #include "share.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
  #include <stdio.h>
  #include "share.h"

  int mcparse(void);
  int mcwrap() { return 1; }
  extern char* mctext;
  extern Map* mcout;
  int mcerror(char *message);
%}

%start program

%union {
    char* identifier;

    struct {
        char* key;
        char* value;
    } entry;

    Map* map;
}

%token <identifier> IDENTIFIER
%token EQUALS
%token END_OF_FILE

%type <entry> entry
%type <map> make_conf

%%

program:                                        {mcout = NULL;}
            | make_conf                         {mcout = $1;}
            ;

make_conf: entry                             {$$ = map_new(64, 0.8); map_insert($$, $1.key, $1.value);}
         | make_conf entry                   {$$ = $1; map_insert($$, $2.key, $2.value);}

entry:   IDENTIFIER EQUALS IDENTIFIER              {$$.key = $1; $$.value = $3;}

%%