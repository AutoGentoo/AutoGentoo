//
// Created by atuser on 10/31/17.
//


#ifndef HACKSAW_COMPILER_USE_H
#define HACKSAW_COMPILER_USE_H

typedef struct __check_use CheckUse;
typedef struct __expr Expression;
typedef struct __atom Atom;
typedef struct __use Use;

#include <tools/vector.h>

typedef enum {
    ALL,
    EQUAL,
    LESS_E,
    GREAT_E,
    LESS,
    GREAT
} atom_st;

typedef enum {
    USE_EXPR,
    SEL_EXPR,
    EXPR_EXPR
} expr_t;

typedef enum {
    USE,
    NOT_USE,
    EXACTLY_ONE,
    AT_LEAST_ONE,
    AT_MOST_ONE
} use_t;

struct __check_use {
    Use* to_check;
    Expression* inner;
};

struct __expr {
    expr_t type;
    CheckUse* c_use; // Use if type == USE_EXPR
    Atom* select; // Use if type == SEL_EXPR
    Vector* expressions; // Use if type == EXPR_EXPR
};

struct __atom {
    char* atom;
    atom_st status;
};

struct __use {
    use_t type;
    char* str;
};

CheckUse* new_check_use (Use* use, Expression* inner);
void add_expression (Vector* list, Vector* exp);
Expression* new_expression(void* ptr, expr_t type);
Atom* new_atom (char* str, atom_st status);
Use* new_use (char* str, use_t type);

/* Debug */
void printf_with_index (char* format, ...);
void debug_expression (Expression* expr);
void print_c_use (CheckUse* c_use);
void print_sel (Atom* selection);

/* Free */
void free_expression (Expression* expr);
void free_check_use (CheckUse* ptr);
void free_use (Use* ptr);
void free_selection (Atom* ptr);

int yyparse(void);

#endif //HACKSAW_COMPILER_USE_H