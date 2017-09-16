/*
 * _string.h
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

#ifndef __AUTOGENTOO__STRING_H__
#define __AUTOGENTOO__STRING_H__

#include <stdio.h>
#include <string.h>

void expand (char* dest, char** src, char* delim, size_t n);
void string_copy (char* dest, char* src);
int check_in (char * arr[], char * el, size_t size);
int _strtok_n (char* s, char delim);
void gen_id (char* id, int len);
int cpy(char *src, char *dest);
char* path_normalize(const char *path);

#endif
