#include <stdio.h>
#include <use.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

CheckUse* new_check_use (Use* use, Expression* inner) {
    CheckUse* out = malloc(sizeof(CheckUse));
    out->to_check = use;
    out->inner = inner;
    return out;
}

Expression* new_expression(void* ptr, expr_t type) {
    Expression* out = malloc (sizeof (Expression));
    out->type = type;
    if (out->type == USE_EXPR) {
        out->c_use = (CheckUse*)ptr;
    }
    else if (out->type == SEL_EXPR) {
        out->select = (Atom*)ptr;
    }
    else if (out->type == EXPR_EXPR) {
        Expression** temp = (Expression**)ptr;
        out->expressions = vector_new(sizeof(Expression), REMOVE | UNORDERED);
        if (temp[0]->type == EXPR_EXPR) {
            add_expression(out->expressions, temp[0]->expressions);
        }
        else {
            vector_add (out->expressions, temp[0]);
        }
        if (temp[1]->type == EXPR_EXPR) {
            add_expression(out->expressions, temp[1]->expressions);
        }
        else {
            vector_add (out->expressions, temp[1]);
        }
    }
    return out;
}

void add_expression (Vector* list, Vector* exp) {
    int i;
    for (i = 0; i != exp->n; i++) {
        Expression* current_expr = (Expression*)vector_get(exp, i);
        if (current_expr->type != EXPR_EXPR) {
            vector_add (list, current_expr);
        }
        else {
            add_expression (list, current_expr->expressions);
        }
    }
    
}

Atom* new_atom (char* str, atom_st status) {
    Atom* out = malloc (sizeof (Atom));
    out->atom = strdup(str);
    out->status = status;
    return out;
}

Use* new_use (char* str, use_t type) {
    Use* out = malloc (sizeof (Use));
    out->str = strdup (str);
    out->type = type;
    return out;
}

static int indent = 0;

void debug_expression (Expression* expr) {
    printf_with_index ("Expression {\n");
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
        printf_with_index ("len: %d\n", expr->expressions->n);
        int i;
        for (i = 0; i != expr->expressions->n; i++) {
            debug_expression((Expression*)vector_get(expr->expressions, i));
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
    printf_with_index("Status: %d\n", selection->status);
    printf_with_index("Atom: %s\n", selection->atom);
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
    debug_expression(c_use->inner);
    indent -= 4;
    printf_with_index("}\n");
}

void printf_with_index (char* format, ...) {
    va_list(args);
    printf("%*c", indent, ' ');
    va_start(args, format);
    vprintf(format, args);
}

void free_expression (Expression* expr) {
    if (expr->type == USE_EXPR) {
        free_check_use (expr->c_use);
    }
    else if (expr->type == SEL_EXPR) {
        free_selection(expr->select);
    }
    else if (expr->type == EXPR_EXPR) {
        int i;
        for (i=0; i != expr->expressions->n; i++) {
            free_expression (vector_get(expr->expressions, i));
        }
    }
    vector_free(expr->expressions);
    free (expr);
}

void free_check_use (CheckUse* ptr) {
    free_expression (ptr->inner);
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