/*
 * util.h
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

#ifndef AUTOGENTOO_UTIL_H
#define AUTOGENTOO_UTIL_H

#include <stdio.h>
#include "aabs.h"
#include <archive.h>


#define MALLOC(p, s, action) do { p = malloc(s); if(p == NULL) { _aabs_alloc_fail(s); action; } } while(0)
#define CALLOC(p, l, s, action) do { p = calloc(l, s); if(p == NULL) { _aabs_alloc_fail(l * s); action; } } while(0)
/* This strdup macro is NULL safe- copying NULL will yield NULL */
#define STRDUP(r, s, action) do { if(s != NULL) { r = strdup(s); if(r == NULL) { _aabs_alloc_fail(strlen(s)); action; } } else { r = NULL; } } while(0)
#define STRNDUP(r, s, l, action) do { if(s != NULL) { r = strndup(s, l); if(r == NULL) { _aabs_alloc_fail(l); action; } } else { r = NULL; } } while(0)
#define FREE(p) do { if (p) free((p)); (p) = NULL; } while(0)
#define ASSERT(cond, action) do { if(!(cond)) { action; } } while(0)

typedef enum {
    AABS_FILE_NOEXIST,
    AABS_FILE_FOUND
} aabs_file_stat_t;

void _aabs_alloc_fail(size_t size);
void* aabs_read_archive (aabs_filelist_t* dest, struct archive** ar, char* file_path, int close);
aabs_time_t _aabs_parsedate (const char* line);
off_t _aabs_str_to_off_t(const char *line);
size_t aabs_str_strip_newline (char* str, size_t len);
char* aabs_fgets (char* dest, int size, FILE* stream);
aabs_file_stat_t aabs_file_exists (char* path);
int aabs_strnrcmp (char* str1, char* str2, size_t n);
int aabs_has_numbers (char* str);
int aabs_has_letters (char* str);

#endif //AUTOGENTOO_UTIL_H
