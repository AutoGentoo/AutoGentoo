%define api.prefix {atom}

%code requires {
  #include "share.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
#include <stdio.h>
#include "share.h"

int atomparse(void);
int atomwrap() { return 1; }
int atomlex();
extern int atomlineno;
extern char* atomtext;
extern P_Atom* atomout;

void atomerror(const char *message);
%}

%start program

%union {
    char* identifier;
    P_Atom* out_type;
    AtomNode* version;
}

%token <identifier> IDENTIFIER
%token <version> VERSION;
%token END_OF_FILE

%type <out_type> atom_simple
%type <out_type> atom_version
%type <out_type> atom
%type <identifier> ident

%%

program:    | atom                      {
                                            atomout = $1;
                                        }
            | END_OF_FILE
            ;

atom :             '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
            |  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |          atom_version     {$$ = $1; $$->blocks = ATOM_BLOCK_NONE;}
            ;

atom_version :       '=' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_E;}
               |     '>' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_G;}
               |     '<' atom_simple       {$$ = $2; $$->range = ATOM_VERSION_L;}
               | '>' '=' atom_simple       {$$ = $3; $$->range = ATOM_VERSION_GE;}
               | '<' '=' atom_simple       {$$ = $3; $$->range = ATOM_VERSION_LE;}
               |         atom_simple       {$$ = $1; $$->range = ATOM_VERSION_ALL;}
               ;

atom_simple :  ident[cat] '/' ident[name] '-' VERSION[version] {
                                            $$ = atom_new ($cat, $name);
                                            $$->version = $version;
                                        }
        ;
            
ident :  ident '-' IDENTIFIER           {
                                            $$ = $1;
                                            strcat ($$, "-");
                                            strcat ($$, $3);
                                        }
            | IDENTIFIER                {
                                            $$ = malloc (256);
                                            $$[0] = 0;
                                            strcat ($$, $1);
                                            free ($1);
                                        }
            ;

%%