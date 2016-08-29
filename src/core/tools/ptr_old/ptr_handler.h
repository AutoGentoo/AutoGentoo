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


#ifndef __AUTOGENTOO_PTR_HANDLER_H__
#define __AUTOGENTOO_PTR_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <mstring.h>

typedef struct _PointerHandler PointerHandler;

struct _PointerHandler
{
  void**           ptrs;
  int              count;
};

/* Size of the current runtime pointer (8, 9, or 10) */
extern int POINTER_SIZE;

/* The main handler for the program */
extern PointerHandler* handler;

/* Set to 1 if you need to see the pointers being allocated/deallocated */
extern int VERBOSE;

/* The size of each palloc () will be increased by this number (default 4) */
extern int PALLOC_ADD;

void handler_init() __attribute__((constructor));

void* palloc (size_t size);

void pfree (void *ptr);

/* Free all items and the ptr */
void array_free (void **ptr);
void array_free_bare (void **ptr);

void set_valid (void *ptr);
int get_valid (void *ptr);

void free_all () __attribute__((destructor));

#ifdef __cplusplus
}
#endif

#endif
