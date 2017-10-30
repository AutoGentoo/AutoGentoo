#include <package/package.h>
#include <package/manifest.h>

Package* package_new (Repository* repo, Category* category, char* name) {
    Package* pkg = malloc (sizeof (Package));
    pkg->category = category;
    pkg->repo = repo;
    pkg->manifest = malloc (sizeof (Manifest));
    char manifest_file[256];
    package_get_file(pkg, package_get_file);
    manifest_parse(pkg, fopen (package_get_file, "r"));
}

void package_get_file (Package* pkg, char* filename) {
    sprintf (filename, "%s/%s/%s/%s", 
        pkg->repo->location,
        pkg->category->name,
        pkg->name,
        filename
    );
}