//
// Created by atuser on 1/5/18.
//

#include <stdio.h>
#include <archive_entry.h>
#include <archive.h>
#include <stdlib.h>
#include "db.h"

static aabs_db_t* local_db;

int main (int argc, char** argv) {
    local_db = aabs_db_new("local", NULL);
    char* db_path = aabs_db_archive_path(local_db);
    if (aabs_file_exists (db_path) == AABS_FILE_NOEXIST) {
        aabs_local_db_write(local_db);
    }
    
    aabs_db_read(local_db);
    
    
    return 0;
}