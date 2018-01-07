/*
 * main.c
 * 
 * Copyright 2018 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


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
