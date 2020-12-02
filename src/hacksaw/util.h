#ifndef HACKSAW_UTIL_H
#define HACKSAW_UTIL_H

#include <stdio.h>
#include "global.h"
#include "object.h"

void prv_mkdir(const char* dir);
void file_copy(char* src, char* dest);
int string_find(char** array, char* element, U32 n);
void fix_path(char* ptr);
char* string_strip(char* str);

typedef struct RefString_prv RefString;

struct RefString_prv {
    REFERENCE_OBJECT
    char* ptr;
};

/**
 * Wrap a string pointer into a
 * reference object.
 * @param str pointer to the string
 * @return reference type of RefString
 */
RefObject* ref_string(char* str);

#endif
