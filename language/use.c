#include <use.h>
#include <stdlib.h>

int main () {
    yyparse();
}

static Expression* current_exp = NULL;

Expression* new_expression() {
    Expression* out = malloc (sizeof (Expression));
    out->atoms = vector_new (sizeof(Atom*), REMOVE | UNORDERED);
    out->children = vector_new (sizeof (Expression*), REMOVE | UNORDERED);
    out->parent = current_exp;
    current_exp = out;
    return out;
}

Atom* new_atom (char* str, atom_st status) {
    Atom* out = malloc (sizeof (Atom));
    out->atom = strdup (str);
    out->status = status;
    return out;
}

void expression_add (void* ptr, expr_t type) {
    switch(type) {
        case EXPR:
        vector_add (current_exp->children, ptr);
        case ATOM:
        vector_add (current_exp->atoms, ptr);
    };
}