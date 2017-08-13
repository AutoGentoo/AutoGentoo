/*
 * list.h
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

#ifndef __AUTOGENTOO__LIST__
#define __AUTOGENTOO__LIST__

#include <stdio.h>
#include <stdlib.h>

#define _LIST_INC_SIZE 5;

typedef struct {
    size_t el_size;
    int total; // Size of the el array
    int used; // Total used/current element
    void ** el;
} _list;

_list * list_init (size_t);
void list_append (_list*, void*);
void list_realloc (_list*);

#endif
