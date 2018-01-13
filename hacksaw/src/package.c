#include <portage/package.h>
#include <portage/manifest.h>
#include <stdlib.h>
#include <tools/string.h>
#include <portage/directory.h>
#include <string.h>
#include <tools/vector.h>

void category_read (Repository* repo, char* name) {
    char cat_dir[256];
    sprintf (cat_dir, "%s/%s", repo->location, name);
    
    StringVector* dirs = get_directories (cat_dir);
    
    int i;
    for (i = 0; i != dirs->n; i++) {
        char key[128];
        char* pkg_name = string_vector_get(dirs, i);
        sprintf (key, "%s/%s", name, pkg_name);
        Package* p = package_new(repo, name, pkg_name);
        map_insert(repo->packages, pkg_name, (void*)p);
    }
    
    string_vector_free(dirs);
}

Package* package_new (Repository* repo, char* category, char* name) {
    Package* pkg = malloc (sizeof (Package));
    pkg->name = strdup(name);
    pkg->category = strdup(category);
    pkg->repo = repo;
    pkg->manifest = malloc (sizeof (Manifest));
    pkg->ebuilds = vector_new (sizeof(EbuildVersion), REMOVE | UNORDERED);
    
    return pkg;
}

void package_get_ebuilds (Package* pkg) {
    manifest_parse(pkg);
    
    int i;
    for (i = 0; i != pkg->manifest->entries->n; i++) {
        ManifestEntry* curr_entry = (ManifestEntry*)vector_get (pkg->manifest->entries, i);
        if (curr_entry->type == EBUILD) {
            vector_add (pkg->ebuilds, curr_entry);
        }
    }
}

void package_get_file (Package* pkg, char* filename) {
    sprintf (filename, "%s/%s/%s/Manifest", 
        pkg->repo->location,
        pkg->category,
        pkg->name
    );
    fix_path (filename);
}