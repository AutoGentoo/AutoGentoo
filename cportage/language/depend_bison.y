%define api.prefix {depend}

%code requires {
  #include "depend.h"
  #include <stdlib.h>
  #include <string.h>
}

%{
#include <stdio.h>
#include <depend.h>

int dependparse(void);
int dependwrap() { return 1; }
int dependlex();
extern int dependlineno;
extern char* dependtext;
extern DependExpression* dependout;

void dependerror(const char *message);
%}

%locations
%start program

%union {
    Atom* atom;
    char* atom_str;
    AtomOpts atom_opts;
    DependExpression* dependexpression;
    Use* use;
    block_t block;
    atom_t version;
    RequireUse use_req;
    Vector* vec;
    AtomSlot slot;
}

%token <use> USE
%token <atom_str> IDENT

%token <block> BLOCKS
%token <version> VERSION

%token END_OF_FILE

%type <dependexpression> expr
%type <atom> select
%type <atom_opts> opts
%type <vec> req_use
%type <use_req> use_flag
%type <use_req> use_token;
%type <use_req> default_use_flag;
%type <slot> slot
%type <block> block
%type <atom> useselect
%type <atom_str> ident;

%%

program:    | expr                      {
                                            dependout = $1;
                                        }
            | END_OF_FILE
            ;

expr:
