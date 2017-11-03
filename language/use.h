//
// Created by atuser on 10/31/17.
//


#ifndef HACKSAW_COMPILER_USE_H
#define HACKSAW_COMPILER_USE_H

typedef struct __expr Expression;
typedef struct __atom Atom;

#include <tools/vector.h>

typedef enum {
    ALL,
    EQUAL,
    LE,
    GE
} atom_st;

typedef enum {
    EXPR,
    ATOM
} expr_t;

struct __expr {
    Expression* parent;
    Vector* children; // Vector<Expression*>
    Vector* atoms; // Vector<Atom*>
};

struct __atom {
    char* atom;
    atom_st status;
};

Expression* new_expression();
Atom* new_atom (char* str, atom_st status);
void expression_add (void* ptr, expr_t type);
int yyparse(void);

#endif //HACKSAW_COMPILER_USE_H