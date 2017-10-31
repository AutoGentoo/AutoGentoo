#include <portage/package.h>
#include <portage/manifest.h>
#include <stdlib.h>
#include <tools/string.h>
#include <portage/directory.h>
#include <string.h>
#include <tools/vector.h>

#define LAMBDA(c_) ({ c_ _;})

Category* category_new (Repository* repo, char* name) {
    Category* cat = malloc (sizeof (Category));
    
    cat->repo = repo;
    cat->packages = vector_new(sizeof (Package*), REMOVE | UNORDERED);
    cat->packages->increment = 16;
    strcpy (cat->name, name);
    
    char cat_dir[256];
    sprintf (cat_dir, "/%s/%s", repo->location, cat->name);
    fix_path (cat_dir);

    StringVector* dirs = get_directories (cat_dir);
    
    int i;
    for (i = 0; i != dirs->n; i++) {
        Package* p = package_new(repo, cat, string_vector_get(dirs, i));
        vector_add(cat->packages, &p);
    }
    
    string_vector_free(dirs);
    
    return cat;
}

Package* package_new (Repository* repo, Category* category, char* name) {
    Package* pkg = malloc (sizeof (Package));
    strcpy (pkg->name, name);
    pkg->category = category;
    pkg->repo = repo;
    pkg->manifest = malloc (sizeof (Manifest));
    
    void* get_path_ptr (char* dest) {
        package_get_file (pkg, dest);
    }
    
    pkg->get_path = get_path_ptr;
    
    manifest_parse(pkg);
    
}

void package_get_file (Package* pkg, char* filename) {
    sprintf (filename, "%s/%s/%s/Manifest", 
        pkg->repo->location,
        pkg->category->name,
        pkg->name
    );
    fix_path (filename);
}