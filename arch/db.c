//
// Created by atuser on 1/6/18.
//

#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include "db.h"
#include "util.h"

aabs_db_t* aabs_db_new (char* name, char* mirror) {
    aabs_db_t* out;
    
    MALLOC (out, sizeof(aabs_db_t), exit (1));
    out->type = 0;
    
    ASSERT(name != NULL, lerror ("db name cannot be NULL");
            FREE (out);
            aabs_errno = AABS_ERR_NULL_PTR;
            return NULL;
           
    );
    STRDUP (out->mirror, mirror, out->type = 1);
    
    out->name = name;
}

void aabs_db_read (aabs_db_t* db) {
    aabs_filelist_t files;
    
    char* db_path = aabs_db_path (db);
    aabs_read_archive (&files, db->obj, db_path, 1);
    free (db);
}

char* aabs_db_path (aabs_db_t* db) {
    char* out = NULL;
    MALLOC (out, strlen(db->name) + strlen(DBPATH) + 3, exit (1));
    sprintf (out, DBPATH "%s%s.db",
             db->type ? "": "sync/",
             db->name);
    
    return out;
}

char* aabs_db_local_pkg_path (aabs_db_t* db, aabs_package_t* pkg) {
    char* out = aabs_db_path(db);
    
    out = realloc (out, strlen(out) + strlen(pkg->name) + strlen(pkg->version) + 12);
    sprintf (&out[strlen(out)], "%s/local/%s-%s", out, pkg->name, pkg->version);
    
    return out;
}