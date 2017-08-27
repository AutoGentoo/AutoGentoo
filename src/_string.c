/*
 * _string.c
 * 
 * Copyright 2017 Unknown <atuser@Hyperion>
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
#include <_string.h>

void expand (char* dest, char** src, char* delim, size_t n) {
    int i;
    for (i=0; i!=(int)n; i++) {
        printf ("%d\n", i);
        strcat (dest, src[i]);
        printf ("%d\n", i);
        fflush(stdout);
        strcat (dest, delim);
    }
}

void string_copy (char* dest, char* src) {
    char* ptr = src;
    
    int orig_str_size = 0;
    
    // Count the number of characters in the original string
    while (*ptr++ != '\0')
        orig_str_size++;        

    printf("Size of the original string: %d\n", orig_str_size);

    // Place the '\0' character at the end of the backup string.
    dest[orig_str_size] = '\0'; 
    ptr = &src[0]; 
    int idx = 0;
    while (*ptr != '\0')
        dest[idx++] = *ptr++;
}

int check_in (char * arr[], char * el, size_t size) {
    int i;
    for (i=0; i != (int)size; i++) {
        if (strcmp (arr[i], el) == 0) {
            return i;
        }
    }
    return -1;
}
