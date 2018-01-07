/*
 * util.c
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

#include <archive_entry.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include <ctype.h>
#include <limits.h>


void _aabs_alloc_fail(size_t size) {
    fprintf(stderr, "alloc failure: could not allocate %zu bytes\n", size);
    aabs_errno = AABS_ERR_MEMORY;
}

void* aabs_read_archive (aabs_filelist_t* dest, struct archive** ar, char* file_path, int close) {
    *ar = archive_read_new();
    struct archive_entry* entry;
    int r;
    
    archive_read_support_filter_all(*ar);
    archive_read_support_format_all(*ar);
    
    r = archive_read_open_filename(*ar, file_path, 10240); // Note 1
    
    if (r != ARCHIVE_OK) {
        aabs_errno = AABS_ERR_DB_READ;
        return NULL;
    }
    
    /* Use the vector api and then copy it over */
    Vector* temp = vector_new (sizeof (aabs_file_t), REMOVE | UNORDERED);
    
    /* Expecting a lot of files */
    temp->increment = 32;
    
    while (archive_read_next_header(*ar, &entry) == ARCHIVE_OK) {
        aabs_file_t t;
        t.name = strdup (archive_entry_pathname (entry));
        t.mode = archive_entry_filetype (entry);
        t.size = archive_entry_size (entry);
        vector_add (temp, &t);
    }
    
    dest->count = temp->n;
    dest->files = temp->ptr;
    free (temp);
    
    if (close) {
        r = archive_read_free(*ar);
        
        if (r != ARCHIVE_OK)
            exit(1);
    }
}

aabs_time_t _aabs_parsedate (const char* line) {
    char *end;
    long long result;
    errno = 0;
    
    result = strtoll(line, &end, 10);
    if(result == 0 && end == line) {
        /* line was not a number */
        errno = EINVAL;
        return 0;
    } else if(errno == ERANGE) {
        /* line does not fit in long long */
        return 0;
    } else if(*end) {
        /* line began with a number but has junk left over at the end */
        errno = EINVAL;
        return 0;
    }
    
    return (aabs_time_t)result;
}

off_t _aabs_str_to_off_t(const char *line) {
    char *end;
    unsigned long long result;
    errno = 0;
    
    /* we are trying to parse bare numbers only, no leading anything */
    if(!isdigit((unsigned char)line[0])) {
        return (off_t)-1;
    }
    result = strtoull(line, &end, 10);
    if(result == 0 && end == line) {
        /* line was not a number */
        return (off_t)-1;
    } else if(result == ULLONG_MAX && errno == ERANGE) {
        /* line does not fit in unsigned long long */
        return (off_t)-1;
    } else if(*end) {
        /* line began with a number but has junk left over at the end */
        return (off_t)-1;
    }
    
    return (off_t)result;
}

size_t aabs_str_strip_newline (char* str, size_t len) {
    if (*str == '\0')
        return 0;
    
    if (len == 0)
        len = strlen(str);
    
    while (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    
    str[len] = '\0';
    return len;
}

char* aabs_fgets (char* dest, int size, FILE* stream) {
    char *ret;
    int errno_save = errno, ferror_save = ferror(stream);
    while((ret = fgets(dest, size, stream)) == NULL && !feof(stream)) {
        if(errno == EINTR) {
            /* clear any errors we set and try again */
            errno = errno_save;
            if(!ferror_save) {
                clearerr(stream);
            }
        } else {
            break;
        }
    }
    return ret;
}

aabs_file_stat_t aabs_file_exists (char* path) {
    return access(path, F_OK) == -1 ? AABS_FILE_NOEXIST : AABS_FILE_FOUND;
}

int aabs_strnrcmp (char* str1, char* str2, size_t n) {
    size_t s1_len = strlen (str1);
    size_t s2_len = strlen (str2);
    
    int i;
    for (i = 1; i <= n; i++) {
        if (str1[s1_len - i] > str2[s2_len - i])
            return -1;
        else if (str1[s1_len - i] < str2[s2_len - i])
            return 1;
    }
    
    return 0;
}

int aabs_has_numbers (char* str) {
    char* s = str;
    while ((*s) != '\0')
        if (isdigit (*s++)) return 1;
    return 0;
}

int aabs_has_letters (char* str) {
    char* s = str;
    while ((*s) != '\0')
        if (!isdigit (*s++)) return 1;
    return 0;
}