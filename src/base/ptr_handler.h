/*
 * ptr_handler.h
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
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


#ifndef __AUTOGENTOO_MEMORY_MANAGMENT__
#define __AUTOGENTOO_MEMORY_MANAGMENT__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

typedef struct _PointerHandler PointerHandler;
typedef struct _PointerGroup PointerGroup;

struct _PointerHandler
{
  PointerGroup**   groups;
  int              size;
  int*             freed;
};

struct _PointerGroup
{
  void**           ptrs;
  int              size;
  int*             is_array;
};

extern int POINTER_SIZE;
extern PointerHandler* mainHandler;
extern int VERBOSE;

void main_init() __attribute__((constructor));

int handler_init (PointerHandler* handler);

PointerGroup* new_group ();

void* paalloc (size_t size, size_t tsize);
void* palloc (size_t size, size_t tsize);
void* gaalloc (size_t size, size_t tsize, PointerGroup* group);
void* galloc (size_t size, size_t tsize, PointerGroup* group);

void pfree (void *ptr);
void ar_pfree (void **ar_ptr);
void gfree (PointerGroup* pgrp);

void set_valid (void *ptr);
int get_valid (void *ptr);

void free_all () __attribute__((destructor));

#ifdef __cplusplus
}
#endif

#endif
