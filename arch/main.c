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
    aabs_db_read(local_db);
    
    return 0;
}