//
// Created by atuser on 10/31/17.
//


#ifndef HACKSAW_COMPILER_USE_H
#define HACKSAW_COMPILER_USE_H

typedef struct __check_use CheckUse;
typedef struct __expr DependExpression;
typedef struct __atom Atom;
typedef struct __use Use;
typedef struct __atom_opts AtomOpts;
typedef struct __require_use RequireUse;

#include <tools/vector.h>

typedef enum {
    ALL,
    EQUAL,
    REVISION,
    LESS_E,
    GREAT_E,
    LESS,
    GREAT,
} atom_t;

typedef enum {
    NO_BLOCK,
    SOFT_BLOCK,
    HARD_BLOCK
} block_t;

typedef enum {
    USE_EXPR,
    SEL_EXPR,
    EXPR_EXPR
} expr_t;

typedef enum {
    YES_USE,
    NO_USE,
    EXACTLY_ONE,
    AT_LEAST_ONE,
    AT_MOST_ONE
} use_t;

typedef enum {
    DISABLED,
    ENABLED,
    SAME, //foo[bar=] bar? ( app-misc/foo[bar] ) !bar? ( app-misc/foo[-bar] )
    OPPOSITE,
    CHECK,
    OPPOSOTE_CHECK,
    DEFAULT_DISABLE,
    DEFAULT_ENABLE
} ruse_t;

struct __check_use {
    Use* to_check;
    DependExpression* inner;
};

struct __atom_opts {
    atom_t status;
    block_t block;
    Vector* required_use;
};

struct __expr {
    expr_t type;
    CheckUse* c_use; // Use if type == USE_EXPR
    Atom* select; // Use if type == SEL_EXPR
    Vector* dependexpressions; // Use if type == EXPR_EXPR
};

struct __atom {
    char* atom;
    AtomOpts opts;
};

struct __use {
    use_t type;
    char* str;
};

struct __require_use {
    char* flag;
    ruse_t status;
};

CheckUse* new_check_use (Use* use, DependExpression* inner);
void add_dependexpression (Vector* list, Vector* exp);
DependExpression* new_dependexpression(void* ptr, expr_t type);
Atom* new_atom (char* str, AtomOpts opts);
void set_atom_opts (AtomOpts* opts, atom_t status, block_t block);
Use* new_use (char* str, use_t type);

/* Debug */
void debug_dependexpression (DependExpression* expr);
void print_c_use (CheckUse* c_use);
void print_sel (Atom* selection);
void print_require_use (RequireUse r);

/* Free */
void free_dependexpression (DependExpression* expr);
void free_check_use (CheckUse* ptr);
void free_use (Use* ptr);
void free_selection (Atom* ptr);

#endif //HACKSAW_COMPILER_USE_H