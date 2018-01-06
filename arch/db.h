//
// Created by atuser on 1/6/18.
//

#ifndef AUTOGENTOO_DB_H
#define AUTOGENTOO_DB_H

#include "aabs.h"
#include "package.h"
#include <archive.h>

typedef struct __aabs_db_t aabs_db_t;

typedef enum {
    DB_BASE = (1 << 0),
    DB_DESC = (1 << 1),
    DB_FILES = (1 << 2),
    DB_INSTALL = (1 << 3),
    DB_
} aabs_db_read_t;


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
    unsigned short type; // 0 for sync; 1 for local
};

aabs_db_t* aabs_db_new (char* name, char* mirror);
void aabs_db_read (aabs_db_t* db);
char* aabs_db_path (aabs_db_t* db);
char* aabs_db_local_pkg_path (aabs_db_t* db, aabs_package_t* pkg);

#endif //AUTOGENTOO_DB_H
