//
// Created by atuser on 10/22/17.
//

#ifndef HACKSAW_STRING_H
#define HACKSAW_STRING_H

#include <stddef.h>

typedef struct __String String;

#ifndef HACKSAW_STRING_INCREMENT
#define HACKSAW_STRING_INCREMENT 32
#endif //HACKSAW_STRING_INCREMENT

struct __String {
    char* ptr;
    size_t n;
    size_t size;
    int increment;
};

String* string_new (size_t start);
void string_append(String* dest, char* str);
void string_append_c(String* dest, int c);
void string_allocate (String* string);
int string_find (char** array, char* element, size_t n);
void string_free (String* string);

#endif //HACKSAW_STRING_H
