//
// Created by atuser on 11/5/17.
//

#ifndef HACKSAW_DEPENDENCY_H
#define HACKSAW_DEPENDENCY_H

#include <portage/package.h>

typedef struct __Dependency Dependency;
typedef struct __PackageSelector PackageSelector;
typedef struct Vector* DependencyVec;  

typedef enum {
    INSTALLED = 0x0,
    NOT_INSTALED = 0x1,
    REQUIRE_REBUILD = 0x2
} depend_t;

struct __PackageSelector {
    char* atom;
    AtomOpts opts;
    AtomSlot slot;
    char* category;
    char* name;
    EbuildVersion version;
};

struct __Dependency {
    DependencyVec DEPEND;  // Vector<Dependency>
    DependencyVec RDEPEND; // Vector<Dependency>
    DependencyVec PDEPEND; // Vector<Dependency>
    DependencyVec HDEPEND; // Vector<Dependency> (EAPI >=5-expirimental)
    Ebuild* package;
    depend_t status;
};

Dependency* dependency_scan (Ebuild* package);
void parse_depend_expression (DependExpression* expr, DependencyVec out_deps);
PackageSelector* get_selector_from_depend (char* dependency_selection);
void print_package_selector (PackageSelector* p);

#endif // HACKSAW_DEPENDENCY_H