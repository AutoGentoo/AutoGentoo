%define api.prefix {package}

%code requires {
  #include "package.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
#include <stdio.h>
#include <package.h>

int packageparse(void);
int packagewrap() { return 1; }
int packagelex();
extern int packagelineno;
extern char* packagetext;
extern PackageSelector* packageout;

void packageerror(const char *message);
%}

%start program

%union {
    char* identifier;
    PackageSelector* sel;
}

%token <identifier> IDENTIFIER
%token END_OF_FILE

%type <sel> atom
%type <identifier> pkg_name

%%

program:    | atom                      {
                                            packageout = $1;
                                        }
            | END_OF_FILE
            ;

atom :  pkg_name[category] '/' pkg_name[name] {
                                            $$ = package_selector_new ($category, $name);
                                        }
        ;

pkg_name :  pkg_name '-' pkg_name       {
                                            $$ = malloc (sizeof(char) * (strlen ($1) + strlen($3)) + 1);
                                            strcat ($$, $1);
                                            strcat ($$, "-");
                                            strcat ($$, $3);
                                        }
            | IDENTIFIER                {$$ = $1;}

%%