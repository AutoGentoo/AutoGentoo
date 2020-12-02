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
  #include "language.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
  #include <stdio.h>
  #include "language.h"

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

%%

program:    entry                             {map_insert(mcout, $1.key, ref_string($1.value));}
           | program entry                    {map_insert(mcout, $2.key, ref_string($2.value));}
           ;

entry:     IDENTIFIER EQUALS IDENTIFIER      {$$.key = $1; $$.value = $3;};

%%