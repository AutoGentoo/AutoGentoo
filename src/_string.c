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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

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

int _strtok_n (char* s, char delim) {
    static int n = 0;
    
    int i, j;
    for (i=n, j=0; s[i] != delim; i++, j++);
    return j;
}

void gen_id (char* id, int len) {
    srandom(time(NULL));  // Correct seeding function for random()
    char c;
    int i;
    for (i=0; i!=len; i++) {
        c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[random () % 62];
        id[i] = c;
    }
    id[len+1] = '\0';
}

int cpy(char *src, char *dest)
{
    int fd_dest, fd_src;
    char buffer[1024];
    int count;

    fd_src = open(src, O_RDONLY);
    if (fd_src == -1) /* Check if file opened */
        return -1;
    fd_dest = open(dest, O_CREAT | S_IRUSR | S_IWUSR | O_WRONLY, 0644);
    if (fd_dest == -1) /* Check if file opened (permissions problems ...) */
    {
        close(fd_src);
        return -1;
    }

    while ((count = read(fd_src, buffer, sizeof(buffer))) != 0)
        write(fd_dest, buffer, count);
}

static void append(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

char * path_normalize(const char *path) {
  if (!path) return NULL;

  char *copy = strdup(path);
  if (NULL == copy) return NULL;
  char *ptr = copy;

  int i;
  for (i = 0; copy[i]; i++) {
    *ptr++ = path[i];
    if ('/' == path[i]) {
      i++;
      while ('/' == path[i]) i++;
      i--;
    }
  }

  *ptr = '\0';

  return copy;
}

char* removeChar(char* str, char c){
    char *dst;
    int i;
     
    for ( i = 0; i < strlen(str) && *str != 0; i++, str++ ){
        if ( *str != c )
            *dst++ = *str;
    }
     
    return &(*str);
}

#define BINMASK 0x14 // 1110

void print_byte(char c) {
    for (int i=7; i!=0; i--) {
        printf("%d", c >> i);
    }
}

void print_bin(void* ptr, size_t s) {
    size_t i;
    for (i=0; i!=s; i++) {
        print_byte(((char*)ptr)[i]);
    }
}