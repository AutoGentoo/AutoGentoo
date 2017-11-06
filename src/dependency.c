#include <portage/package.h>
#include <tools/vector.h>
#include <portage/dependency.h>
#include <language/share.h>
#include <string.h>

Dependency* dependency_scan (Ebuild* package) {
    return NULL;
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