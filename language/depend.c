#include <stdio.h>
#include <depend.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

CheckUse* new_check_use (Use* use, DependExpression* inner) {
    CheckUse* out = malloc(sizeof(CheckUse));
    out->to_check = use;
    out->inner = inner;
    return out;
}

DependExpression* new_dependexpression(void* ptr, expr_t type) {
    DependExpression* out = malloc (sizeof (DependExpression));
    out->type = type;
    if (out->type == USE_EXPR) {
        out->c_use = (CheckUse*)ptr;
    }
    else if (out->type == SEL_EXPR) {
        out->select = (Atom*)ptr;
    }
    else if (out->type == EXPR_EXPR) {
        DependExpression** temp = (DependExpression**)ptr;
        out->dependexpressions = vector_new(sizeof(DependExpression), REMOVE | UNORDERED);
        if (temp[0]->type == EXPR_EXPR) {
            add_dependexpression(out->dependexpressions, temp[0]->dependexpressions);
        }
        else {
            vector_add (out->dependexpressions, temp[0]);
        }
        if (temp[1]->type == EXPR_EXPR) {
            add_dependexpression(out->dependexpressions, temp[1]->dependexpressions);
        }
        else {
            vector_add (out->dependexpressions, temp[1]);
        }
    }
    return out;
}

void add_dependexpression (Vector* list, Vector* exp) {
    int i;
    for (i = 0; i != exp->n; i++) {
        DependExpression* current_expr = (DependExpression*)vector_get(exp, i);
        if (current_expr->type != EXPR_EXPR) {
            vector_add (list, current_expr);
        }
        else {
            add_dependexpression (list, current_expr->dependexpressions);
        }
    }
    
}

Atom* new_atom (char* str, atom_t status, block_t block) {
    Atom* out = malloc (sizeof (Atom));
    out->atom = strdup(str);
    out->status = status;
    out->block = block;
    return out;
}

Use* new_use (char* str, use_t type) {
    Use* out = malloc (sizeof (Use));
    out->str = strdup (str);
    out->type = type;
    return out;
}

static int indent = 0;

void debug_dependexpression (DependExpression* expr) {
    printf_with_index ("DependExpression {\n");
    indent += 4;
    switch(expr->type) {
        case USE_EXPR:
        print_c_use(expr->c_use);
        break;
        case SEL_EXPR:
        print_sel (expr->select);
        break;
        case EXPR_EXPR:
        printf_with_index ("[\n");
        indent += 4;
        printf_with_index ("len: %d\n", expr->dependexpressions->n);
        int i;
        for (i = 0; i != expr->dependexpressions->n; i++) {
            debug_dependexpression((DependExpression*)vector_get(expr->dependexpressions, i));
        }
        indent -= 4;
        printf_with_index ("]\n");
        break;
        default:
        printf_with_index ("not initialized\n");
    }
    indent -= 4;
    printf_with_index("}\n");
}

void print_sel (Atom* selection) {
    printf_with_index ("Atom {\n");
    indent += 4;
    printf_with_index("Atom: %s\n", selection->atom);
    printf_with_index("Status: %d\n", selection->status);
    printf_with_index("Block: %d\n", selection->block);
    indent -= 4;
    printf_with_index("}\n");
}

void print_c_use (CheckUse* c_use) {
    printf_with_index ("CheckUse {\n");
    indent += 4;
    printf_with_index ("Use {\n");
    indent += 4;
    printf_with_index("Flag: %s\n", c_use->to_check->str);
    printf_with_index("Type: %d\n", c_use->to_check->type);
    indent -= 4;
    printf_with_index("}\n");
    debug_dependexpression(c_use->inner);
    indent -= 4;
    printf_with_index("}\n");
}

void printf_with_index (char* format, ...) {
    va_list(args);
    printf("%*c", indent, ' ');
    va_start(args, format);
    vprintf(format, args);
}

void free_dependexpression (DependExpression* expr) {
    if (expr->type == USE_EXPR) {
        free_check_use (expr->c_use);
    }
    else if (expr->type == SEL_EXPR) {
        free_selection(expr->select);
    }
    else if (expr->type == EXPR_EXPR) {
        int i;
        for (i=0; i != expr->dependexpressions->n; i++) {
            free_dependexpression (vector_get(expr->dependexpressions, i));
        }
    }
    vector_free(expr->dependexpressions);
    free (expr);
}

void free_check_use (CheckUse* ptr) {
    free_dependexpression (ptr->inner);
    free_use (ptr->to_check);
    free(ptr);
}

void free_use (Use* ptr) {
    free (ptr->str);
    free (ptr);
}

void free_selection (Atom* ptr) {
    free(ptr->atom);
    free (ptr);
}