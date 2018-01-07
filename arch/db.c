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
#include "deps.h"
#include <stddef.h>

aabs_db_read_handler_t db_pkg_read_offsets[] = {
        {"%NAME%", offsetof (aabs_pkg_t, name)},
        {"%VERSION%", offsetof (aabs_pkg_t, version)},
        {"%BASE%", offsetof (aabs_pkg_t, base)},
        {"%DESC%", offsetof (aabs_pkg_t, desc)},
        {"%GROUPS%", offsetof (aabs_pkg_t, groups), AABS_DB_HANDLE_TYPE_SVEC},
        {"%URL%", offsetof (aabs_pkg_t, url)},
        {"%LICENSE%", offsetof (aabs_pkg_t, licenses), AABS_DB_HANDLE_TYPE_SVEC},
        {"%ARCH%", offsetof (aabs_pkg_t, arch)},
        {"%BUILDDATE%", offsetof (aabs_pkg_t, builddate), AABS_DB_HANDLE_TYPE_STRING, &_aabs_parsedate},
        {"%INSTALLDATE%", offsetof (aabs_pkg_t, installdate), AABS_DB_HANDLE_TYPE_STRING, &_aabs_parsedate},
        {"%PACKAGER%", offsetof (aabs_pkg_t, packager)},
        {"%REASON%", offsetof (aabs_pkg_t, reason), AABS_DB_HANDLE_TYPE_STRING, &atoi},
        {"%VALIDATION%", offsetof (aabs_pkg_t, validation), AABS_DB_HANDLE_TYPE_SVEC, NULL, &aabs_validation_get},
        {"%SIZE%", offsetof (aabs_pkg_t, isize), AABS_DB_HANDLE_TYPE_STRING, &_aabs_str_to_off_t},
        {"%REPLACES%", offsetof (aabs_pkg_t, replaces), AABS_DB_HANDLE_TYPE_DEP},
        {"%DEPENDS%", offsetof (aabs_pkg_t, depends), AABS_DB_HANDLE_TYPE_DEP},
        {"%OPTDEPENDS%", offsetof (aabs_pkg_t, optdepends), AABS_DB_HANDLE_TYPE_DEP},
        {"%CONFLICTS%", offsetof (aabs_pkg_t, conflicts), AABS_DB_HANDLE_TYPE_DEP},
        {"%PROVIDES%", offsetof (aabs_pkg_t, provides), AABS_DB_HANDLE_TYPE_DEP},
        {NULL, 0, AABS_DB_HANDLE_TYPE_STRING, NULL, NULL}
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
    char* db_path = aabs_db_archive_path (db);
    
    db->obj = archive_read_new();
    struct archive_entry* entry;
    int r;
    
    archive_read_support_filter_all(db->obj);
    archive_read_support_format_all(db->obj);
    
    r = archive_read_open_filename(db->obj, db_path, 10240); // Note 1
    
    if (r != ARCHIVE_OK) {
        aabs_errno = AABS_ERR_DB_READ;
        lerror ("Failed to read db archive");
        return;
    }
    
    aabs_pkg_t* curr_pkg = NULL;
    
    while (archive_read_next_header(db->obj, &entry) == ARCHIVE_OK) {
        char* full_path = (char*)archive_entry_pathname (entry);
        if (strcmp (full_path, "ALPM_DB_VERSION") == 0) {
            continue;
        }
        
        char* file_name = strchr(full_path, '/') + 1;
        *(file_name - 1) = '\0'; // Split the paths
        
        /* Once we enter a new directory we are parsing for a different package */
        if (*file_name == '\0') {
            curr_pkg = malloc (sizeof (aabs_pkg_t));
            memset (curr_pkg, 0, sizeof (aabs_pkg_t));
            continue;
        }
        
        if (strcmp (file_name, "desc") == 0) {
            size_t current_size = (size_t)archive_entry_size (entry);
            char* buffer = malloc (current_size + 4);
            ssize_t size = archive_read_data(db->obj, buffer, current_size);
    
            FILE* desc_fp = fmemopen (buffer, current_size, "r");
            char* line;
            size_t len;
            ssize_t read_size = getline (&line, &len, desc_fp);
            char* line_cpy;
            
            while (read_size != 0) {
                aabs_db_read_handler_t handler;
                if (line[0] == '\n') {
                    read_size = getline (&line, &len, desc_fp);
                    continue;
                }
                
                line[strlen (line) - 1] = '\0';
                size_t current_line_length = strlen(line);
                if (strlen(line) == 0)
                    break;
                
                int i;
                for (i = 0, handler = db_pkg_read_offsets[i];
                     handler.desc_header != NULL
                        && strcmp (handler.desc_header, line) != 0;
                     ++i, handler = db_pkg_read_offsets[i]);
                
                if (handler.desc_header == NULL) {
                    fprintf (stderr, "could not recognize header %s for package %s\n", line, full_path);
                    break;
                }
                
                void* dest = curr_pkg + handler.offset;
                void* dest_buffer = dest;
                
                aabs_depend_t* current_depend;
                aabs_svec_t* svec_temp;
                
                switch (handler.type) {
                    case AABS_DB_HANDLE_TYPE_STRING:
                        getline (&line, &len, desc_fp);
                        if (handler.single_handler != NULL) {
                            *((aabs_int64_t*)dest) = (*handler.single_handler) (line);
                            break;
                        }
                        *(char**)dest = strdup (line);
                        break;
                    case AABS_DB_HANDLE_TYPE_SVEC:
                        if (handler.list_handler != NULL) {
                            dest_buffer = &svec_temp;
                        }
        
                        *(aabs_svec_t**)dest_buffer = string_vector_new();
                        while (1) {
                            getline (&line, &len, desc_fp);
                            line[strlen (line) - 1] = '\0';
                            if (*line == '\0')
                                break;
                            string_vector_add (*(aabs_svec_t**)dest_buffer, line);
                        }
                        
                        if (handler.list_handler != NULL) {
                            *((int*)dest) = handler.list_handler(*(aabs_svec_t**)dest_buffer);
                            string_vector_free (*(aabs_svec_t**)dest_buffer);
                        }
                        break;
                    case AABS_DB_HANDLE_TYPE_DEP:
                        *((aabs_vec_t**)dest)  = vector_new (sizeof (aabs_depend_t*), REMOVE | UNORDERED);
                        while (1) {
                            getline (&line, &len, desc_fp);
                            line[strlen (line) - 1] = '\0';
                            if (*line == '\0')
                                break;
                            current_depend = aabs_dep_from_str (line);
                            //vector_add (*dep_dest, &current_depend);
                        }
                        break;
                    default:
                        break;
                }
                read_size = getline (&line, &len, desc_fp);
            }
            
            fclose (desc_fp);
            free (buffer);
        }
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
        }
        else if(strcmp(data, "md5") == 0) {
            out |= AABS_PKG_VALIDATION_MD5SUM;
        }
        else if(strcmp(data, "sha256") == 0) {
            out |= AABS_PKG_VALIDATION_SHA256SUM;
        }
        else if(strcmp(data, "pgp") == 0) {
            out |= AABS_PKG_VALIDATION_SIGNATURE;
        }
        else {
            lerror("unknown validation type for package %s: %s\n",
                   "", (const char *)data);
        }
    }
    
    return out;
}
