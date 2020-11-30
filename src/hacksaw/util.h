#ifndef HACKSAW_UTIL_H
#define HACKSAW_UTIL_H

#include <stdio.h>

void prv_mkdir(const char* dir);

void file_copy(char* src, char* dest);

int string_find(char** array, char* element, size_t n);

void fix_path(char* ptr);

char* string_strip(char* str);

#endif
