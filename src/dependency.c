#include <portage/package.h>
#include <tools/vector.h>
#include <portage/dependency.h>
#include <language/share.h>
#include <string.h>

Dependency* dependency_scan (Ebuild* package) {
    Dependency* out = malloc (sizeof (Dependency));
    
    Dependency** ar[] = {
        &out->DEPEND,
        &out->RDEPEND,
        &out->PDEPEND
    };
    
    char* ar_str[] {
        "DEPEND",
        "RDEPEND",
        "PDEPEND"
    };
    
    int i;
    for (i=0; i!=3; i++) {
        char* temp_str = conf_get(package->metadata, "", ar_str);
        DependExpression* temp = depend_parse(temp_str);
        *ar[i] = parse_depend_expression(temp);
    }
}

Vector* parse_depend_expression (DependExpression* expr) {
    switch (expr->type) {
        case USE_EXPR:
        //if ()
        break;
        case SEL_EXPR:
        
        break;
        case EXPR_EXPR:
        
        break;
    }
}

PackageSelector* get_selector_from_depend (char* dependency_selection) {
    DependExpression* expression = depend_parse(dependency_selection);
    if (expression->type != SEL_EXPR) {
        printf ("invalid selection '%s'", dependency_selection);
        return NULL;
    }
    PackageSelector* out = malloc(sizeof (PackageSelector));
    memcpy((Atom*)out, expression->select, sizeof (Atom));
    
    AtomSelector* a_sel = atom_parse(out->atom);
    memcpy ((AtomSelector*)&out->category, a_sel, sizeof (AtomSelector));
    return out;
}

void print_package_selector (PackageSelector* p) {
    print_sel((Atom*)p);
    print_atom_selector((AtomSelector*)&p->category);
}