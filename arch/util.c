//
// Created by atuser on 1/6/18.
//

#include <archive_entry.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


void _aabs_alloc_fail(size_t size) {
    fprintf(stderr, "alloc failure: could not allocate %zu bytes\n", size);
    aabs_errno = AABS_ERR_MEMORY;
}

void* aabs_read_archive (aabs_filelist_t* dest, struct archive* ar, char* file_path, int close) {
    ar = archive_read_new();
    struct archive_entry* entry;
    int r;
    
    archive_read_support_filter_all(ar);
    archive_read_support_format_all(ar);
    
    r = archive_read_open_filename(ar, file_path, 10240); // Note 1
    
    if (r != ARCHIVE_OK) {
        aabs_errno = AABS_ERR_DB_READ;
        return NULL;
    }
    
    /* Use the vector api and then copy it over */
    Vector* temp = vector_new (sizeof (aabs_file_t), REMOVE | UNORDERED);
    
    /* Expecting a lot of files */
    temp->increment = 32;
    
    while (archive_read_next_header(ar, &entry) == ARCHIVE_OK) {
        aabs_file_t t;
        t.name = strdup (archive_entry_pathname (entry));
        t.mode = archive_entry_filetype (entry);
        t.size = archive_entry_size (entry);
        vector_add (temp, &t);
    }
    dest = temp->ptr;
    free (temp);
    
    if (close) {
        r = archive_read_free(ar);
        
        if (r != ARCHIVE_OK)
            exit(1);
    }
}