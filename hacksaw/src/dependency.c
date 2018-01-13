#include <portage/package.h>
#include <tools/vector.h>
#include <portage/dependency.h>
#include <language/share.h>
#include <string.h>
#include <stdlib.h>

Dependency* dependency_scan (Ebuild* package) {
    Dependency* out = malloc (sizeof (Dependency));
    
    DependencyVec* ar[] = {
        &out->DEPEND,
        &out->RDEPEND,
        &out->PDEPEND,
        &out->HDEPEND
    };
    
    char* ar_str[] = {
        "DEPEND",
        "RDEPEND",
        "PDEPEND",
        "HDEPEND"
    };
    
    int i;
    for (i=0; i!=sizeof(ar_str) / sizeof(ar_str[0]); i++) {
        *ar[i] = (DependencyVec)vector_new (sizeof (Ebuild*), REMOVE | UNORDERED);
        char* temp_str = conf_get(package->metadata, "", ar_str[i]);
        DependExpression* temp = depend_parse(temp_str);
        parse_depend_expression(temp, *ar[i]);
    }
}

PackageSelector* prv_selector_from_depend_atom (Atom* in) {
    PackageSelector* out = malloc(sizeof (PackageSelector));
    memcpy((Atom*)out, in, sizeof (Atom));
    
    AtomSelector* a_sel = atom_parse(out->atom);
    memcpy ((AtomSelector*)&out->category, a_sel, sizeof (AtomSelector));
    return out;
}

void parse_depend_expression (DependExpression* expr, DependencyVec out_deps) {
    switch (expr->type) {
        case USE_EXPR:
        //expr->c_use->to_check
        break;
        case SEL_EXPR:
            
            expr->select->opts.status;
            break;
        case EXPR_EXPR:
            parse_depend_expression (vector_get(expr->dependexpressions, 0), out_deps);
            parse_depend_expression (vector_get(expr->dependexpressions, 1), out_deps);
        break;
    }
}

PackageSelector* get_selector_from_depend (char* dependency_selection) {
    DependExpression* expression = depend_parse(dependency_selection);
    if (expression->type != SEL_EXPR) {
        printf ("invalid selection '%s'", dependency_selection);
        return NULL;
    }
    
    return prv_selector_from_depend_atom(expression->select);
}

void print_package_selector (PackageSelector* p) {
    print_sel((Atom*)p);
    print_atom_selector((AtomSelector*)&p->category);
}