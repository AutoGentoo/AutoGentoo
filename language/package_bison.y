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
    PackageSelectorVersion version;
    int r;
}

%token <identifier> IDENTIFIER
%token <r> REVISION;
%token <version> VERSION;
%token END_OF_FILE

%type <sel> atom
%type <identifier> pkg_name
%type <version> pkg_version

%%

program:    | atom                      {
                                            packageout = $1;
                                        }
            | END_OF_FILE
            ;

atom :  pkg_name[cat] '/' pkg_name[name] '-' pkg_version[version] {
                                            $$ = package_selector_new ($cat, $name);
                                            $$->version = $version;
                                        }
        ;
            
pkg_name :  pkg_name '-' IDENTIFIER     {
                                            $$ = $1;;
                                            strcat ($$, "-");
                                            strcat ($$, $3);
                                        }
            | IDENTIFIER                {
                                            $$ = malloc (256);
                                            strcat ($$, $1);
                                        }
            ;

pkg_version : VERSION REVISION      {
                                            $$ = $1;
                                            $$.revision = $2;
                                        }
            | VERSION                   {
                                            $$ = $1;
                                        }
            ;

%%