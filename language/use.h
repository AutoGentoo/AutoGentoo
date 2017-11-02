//
// Created by atuser on 10/31/17.
//


#ifndef HACKSAW_COMPILER_USE_H
#define HACKSAW_COMPILER_USE_H

typedef struct __expr expression;
typedef enum {
    ALL,
    EQUAL,
    LE,
    GE
} atom_st;

struct __expr {
    expression* parent;
    atom_st status;
    char* atom;
};

int yyparse(void);
extern int sym[26];

#endif //HACKSAW_COMPILER_USE_H