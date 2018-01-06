//
// Created by atuser on 1/6/18.
//

#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include "db.h"
#include "util.h"
#include "aabs.h"
#include <stddef.h>

aabs_db_read_handler_t db_pkg_read_offsets[] = {
        {"NAME", offsetof (aabs_pkg_t, name)},
        {"VERSION", offsetof (aabs_pkg_t, version)},
        {"BASE", offsetof (aabs_pkg_t, base)},
        {"DESC", offsetof (aabs_pkg_t, desc)},
        {"GROUPS", offsetof (aabs_pkg_t, groups), AABS_DB_HANDLE_TYPE_SVEC},
        {"URL", offsetof (aabs_pkg_t, url)},
        {"LICENSE", offsetof (aabs_pkg_t, licenses), AABS_DB_HANDLE_TYPE_SVEC},
        {"ARCH", offsetof (aabs_pkg_t, arch)},
        {"BUILDDATE", offsetof (aabs_pkg_t, builddate), AABS_DB_HANDLE_TYPE_STRING, _aabs_parsedate},
        {"INSTALLDATE", offsetof (aabs_pkg_t, installdate), AABS_DB_HANDLE_TYPE_STRING, _aabs_parsedate},
        {"PACKAGER", offsetof (aabs_pkg_t, packager)},
        {"REASON", offsetof (aabs_pkg_t, reason), AABS_DB_HANDLE_TYPE_STRING, (aabs_int64_t (*)(const char*))atoi},
        {"VALIDATION", offsetof (aabs_pkg_t, validation), AABS_DB_HANDLE_TYPE_SVEC, NULL, (aabs_int64_t (*)(aabs_svec_t*))aabs_validation_get},
        {"SIZE", offsetof (aabs_pkg_t, isize), AABS_DB_HANDLE_TYPE_STRING, _aabs_str_to_off_t},
        {"REPLACES", offsetof (aabs_pkg_t, replaces), AABS_DB_HANDLE_TYPE_DEP},
        {"DEPENDS", offsetof (aabs_pkg_t, depends), AABS_DB_HANDLE_TYPE_DEP},
        {"OPTDEPENDS", offsetof (aabs_pkg_t, optdepends), AABS_DB_HANDLE_TYPE_DEP},
        {"CONFLICTS", offsetof (aabs_pkg_t, conflicts), AABS_DB_HANDLE_TYPE_DEP},
        {"PROVIDES", offsetof (aabs_pkg_t, provides), AABS_DB_HANDLE_TYPE_DEP}
};

aabs_db_t* aabs_db_new (char* name, char* mirror) {
    aabs_db_t* out;
    MALLOC (out, sizeof(aabs_db_t), exit (1));
    out->packages = map_new (sizeof (aabs_pkg_t*), 64);
    out->type = 1;
    
    ASSERT(name != NULL, lerror ("db name cannot be NULL");
            FREE (out);
            aabs_errno = AABS_ERR_NULL_PTR;
            return NULL;
           
    );
    
    if (mirror) {
        out->mirror = strdup(mirror);
    }
    
    out->name = name;
}

void aabs_db_read (aabs_db_t* db) {
    aabs_filelist_t files;
    
    char *splt_pkg, *current_file, *splt_file, *db_path;
    db_path = aabs_db_archive_path (db);
    
    linfo (db_path);
    aabs_read_archive (&files, &db->obj, db_path, 0);
    
    aabs_pkg_t* current_pkg;
    
    int i;
    for (i = 0; i != files.count; i++) {
        current_file = files.files[i].name;
        splt_pkg = strtok (current_file, "/");
        splt_file = strtok (NULL, "");
        
        if (strcmp (splt_file, "desc") == 0) {
            
        }
        //map_insert(db->packages, )
    }
    
    free (db_path);
}

char* aabs_db_archive_path (aabs_db_t* db) {
    char* out = NULL;
    MALLOC (out, strlen(db->name) + strlen(DBPATH) + 32, exit (1));
    sprintf (out, DBPATH "%s/%s.db",
             db->type ? "": "/sync",
             db->name);
    
    return out;
}

char* aabs_db_path (aabs_db_t* db) {
    char* out = NULL;
    MALLOC (out, strlen(DBPATH) + 32, exit (1));
    sprintf (out, DBPATH "%s/", db->type ? "/local": "/sync");
    
    return out;
}

char* aabs_local_db_pkgpath(aabs_db_t* db,
                            aabs_pkg_t* info,
                            const char* filename) {
    size_t len;
    char *pkgpath;
    const char *dbpath;
    
    dbpath = aabs_db_path(db);
    len = strlen(dbpath) + strlen(info->name) + strlen(info->version) + 3;
    len += filename ? strlen(filename) : 0;
    MALLOC(pkgpath, len, exit (1));
    sprintf(pkgpath, "%s%s-%s/%s", dbpath, info->name, info->version,
            filename ? filename : "");
    return pkgpath;
}

void aabs_local_write_db (aabs_db_t* db, aabs_pkg_t* pkg, aabs_db_read_t opts) {
    FILE* fp = NULL;
    
    char* target_dir = aabs_local_db_pkgpath (db, pkg, NULL);
    
    if (opts & DB_BASE) {
        prv_mkdir (target_dir);
    }
    
    if (opts & DB_DESC) {
    
    }
}

aabs_pkgvalidation_t aabs_validation_get (aabs_svec_t* vec) {
    aabs_pkgvalidation_t out = (aabs_pkgvalidation_t)0;
    
    int i;
    for(i = 0; i != vec->n; i++) {
        char* data = string_vector_get(vec, i);
        if(strcmp(data, "none") == 0) {
            out |= AABS_PKG_VALIDATION_NONE;
        } else if(strcmp(data, "md5") == 0) {
            out |= AABS_PKG_VALIDATION_MD5SUM;
        } else if(strcmp(data, "sha256") == 0) {
            out |= AABS_PKG_VALIDATION_SHA256SUM;
        } else if(strcmp(data, "pgp") == 0) {
            out |= AABS_PKG_VALIDATION_SIGNATURE;
        } else {
            lerror("unknown validation type for package %s: %s\n",
                   "", (const char *)data);
        }
    }
    
    return out;
}