#include <portage/package.h>
#include <portage/manifest.h>
#include <stdlib.h>
#include <tools/string.h>
#include <portage/directory.h>
#include <string.h>
#include <tools/vector.h>

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
    pkg->ebuilds = vector_new (sizeof(EbuildVersion), REMOVE | UNORDERED);
    
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

void ebuild_get_version (Ebuild* ebuild, char* dest) {
    char revision_dest[16];
    revision_dest[0] = 0;
    if (ebuild->version->revision != 0) {
        sprintf (revision_dest, "-r%d", ebuild->version->revision);
    }
    *dest = 0;
    
    int i;
    for (i = 0; i != ebuild->version->version->n; i++) {
        char t[16];
        sprintf (t, "%d", *(int*)vector_get(ebuild->version->version, i));
        strcat (dest, t);
        if (i + 1 != ebuild->version->version->n) {
            strcat (dest, ".");
        }
    }
    strcat (dest, ebuild->version->suffix);
    strcat (dest, revision_dest);
}

void ebuild_get_metadata (Ebuild* ebuild, char* dest) {
    char version_temp [64];
    ebuild_get_version (ebuild, version_temp);
    sprintf (dest, "%s/metadata/md5-cache/%s/%s-%s", 
            ebuild->parent->repo->location,
            ebuild->parent->category->name,
            ebuild->parent->name,
            version_temp);
    fix_path (dest);
}

Ebuild* ebuild_new (Package* pkg, EbuildVersion* find) {
    int i;
    Ebuild* out;
    for (i = 0; i != pkg->ebuilds->n; i++) {
        EbuildVersion* current = vector_get(pkg->ebuilds, i);
        if (memcmp(current, find, sizeof (EbuildVersion)) == 0) {
            out = malloc (sizeof (Ebuild));
            out->parent = pkg;
            out->version = current;
            char meta_file[256];
            ebuild_get_metadata (out, meta_file);
            out->metadata = conf_new (meta_file);
            return out;
        }
    }
    return NULL; // Not found
}