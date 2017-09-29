/*
 * update.c
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
#include <stdlib.h>
#include <string.h>

int str_find (char* str, char c, size_t size) {
    int i;
    for (i=0; i!=(int)size; i++) {
        if (str[i] == c) {
            return i;
        }
    }
    return -1;
}

void str_substring (char* dest, char* src, int start, int end) {
    strncpy(dest, src + start, end - start);
}

int main (int argc, char** argv) {
    char line[1024];
    
    printf ("emerge -q --oneshot --nodeps --deep");
    
    
    while (scanf (" %[^\n]s", line) != -1) {
        getchar();
        if (line[0] != '[') {
            continue;
        }
        
        if (strncmp (line + 1, "ebuild", 6) != 0 && line[10] == 'R') {
            continue;
        }
        char pkgbuf[1024];
        str_substring (pkgbuf, line, str_find (line, ']', 20) + 2, strlen (line));
        char ebuild_update[1024];
        memset(&ebuild_update[0], 0, sizeof(ebuild_update));
        strncpy (ebuild_update, pkgbuf, str_find (pkgbuf, ' ', strlen(pkgbuf)));
        printf (" =%s", ebuild_update);
    }
    printf ("\n");
    
    return 0;
}