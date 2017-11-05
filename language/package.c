#include <stdio.h>
#include <package.h>
#include <stdlib.h>
#include <string.h>
#include <share.h>

PackageSelector* package_selector_new (char* cat, char* name) {
    PackageSelector* out = malloc (sizeof (PackageSelector));
    out->category = strdup (cat);
    out->name = strdup (name);
    return out;
}

void set_package_selector_version (PackageSelectorVersion* v, char* version_str, int r) {
    char* to_use = strchr (version_str, '_');
    if (to_use != NULL) {
        *to_use = 0;
        v->suffix = strdup (to_use + 1);
    }
    v->version = parse_version(version_str);
    v->revision = r;
}

Vector* parse_version (char* v_str) {
    Vector* out = vector_new (sizeof (int), KEEP | ORDERED);
    
    char* buf;
    int temp;
    for (buf = strtok (v_str, "."); buf != NULL; buf = strtok (NULL, ".")) {
        sscanf (buf, "%d", &temp);
        vector_add (out, &temp);
    }
    return out;
}

void print_package_selector (PackageSelector* pkg) {
    printf_with_indent("PackageSelector {\n");
    indent += 4;
    printf_with_indent ("category: %s\n", pkg->category);
    printf_with_indent ("name: %s\n", pkg->name);
    print_package_version (pkg->version.version);
    printf_with_indent ("revision: %d\n", pkg->version.revision);
    printf_with_indent ("suffix: %s\n", pkg->version.suffix);
    indent -= 4;
    printf_with_indent ("}\n");
}

void print_package_version (Vector* ver) {
    int i;
    if (ver == NULL) {
        return;
    }
    printf_with_indent ("version: ");
    for (i=0; i!=ver->n; i++) {
        printf ("%d", *(int*)vector_get (ver, i));
        if (i + 1 != ver->n)
            printf (".");
    }
    printf ("\n");
}

void free_package_selector (PackageSelector* ptr) {
    if (ptr->version.version != NULL) {
        vector_free (ptr->version.version);
    }
    free(ptr->category);
    free(ptr->name);
    free(ptr);
}