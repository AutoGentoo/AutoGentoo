//
// Created by atuser on 1/6/18.
//

#ifndef AUTOGENTOO_DB_H
#define AUTOGENTOO_DB_H

#include "aabs.h"
#include "package.h"
#include "util.h"
#include <archive.h>

typedef struct __aabs_db_t aabs_db_t;
typedef struct _aabs_db_read_handler_t aabs_db_read_handler_t;

typedef enum {
    DB_BASE = (1 << 0),
    DB_DESC = (1 << 1) | DB_BASE,
    DB_FILES = (1 << 2) | DB_BASE,
    DB_INSTALL = (1 << 3) | DB_BASE,
    DB_ALL =  DB_BASE | DB_FILES | DB_INSTALL
} aabs_db_read_t;

typedef enum {
    AABS_DB_HANDLE_TYPE_STRING,
    AABS_DB_HANDLE_TYPE_SVEC,
    AABS_DB_HANDLE_TYPE_DEP
} aabs_db_read_handle_type_t;

struct _aabs_db_read_handler_t {
    char* desc_header;
    size_t offset;
    aabs_db_read_handle_type_t type;
    int (*single_handler)(const char* line);
    int (*list_handler) (aabs_svec_t* vec);
    aabs_int64_t (*single_handler_large)(const char* line);
};

struct aabs_db_backup {
    char* filename;
    char* md5sum;
};

typedef enum {
    AABS_DB_TYPE_SYNC,
    AABS_DB_TYPE_LOCAL
} aabs_db_type;

/*
 * Two types of db
 * sync and local
 * sync with what we download from our mirror
 * local is what we generate from /var/lib/pacman/local/
 * both with be plain archives with no compression
 * If the mirror is not NULL then it will be a sync otherwise
 * it is local
 */

struct __aabs_db_t {
    char* name;
    char* mirror;
    struct archive* obj;
    aabs_map_t* packages;
    aabs_db_type type;
    
    aabs_filelist_t files;
};

aabs_db_t* aabs_db_new (char* name, char* mirror);
void aabs_local_db_write (aabs_db_t* db);
void aabs_db_read (aabs_db_t* db);
char* aabs_db_path (aabs_db_t* db);
char* aabs_db_archive_path (aabs_db_t* db);
char* aabs_local_db_pkgpath(aabs_db_t* db,
                            aabs_pkg_t* info,
                            const char* filename);

void aabs_local_write_db (aabs_db_t* db,
                          aabs_pkg_t* pkg,
                          aabs_db_read_t opts);
aabs_pkgvalidation_t aabs_validation_get (aabs_svec_t* vec);
int aabs_db_handler_get (char* name);

#define READ_NEXT(x) ({ \
    size_t ret_val;\
    if(aabs_fgets(line, sizeof(line), fp) == NULL) break; \
    ret_val = aabs_str_strip_newline(line, 0); \
ret_val;})

#define READ_NEXT_NO_BREAK() ({ \
    size_t ret_val;\
    ret_val = aabs_str_strip_newline(line, 0); \
ret_val;})

#endif //AUTOGENTOO_DB_H
