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
#include <stdarg.h>
#include <signal.h>

enum BOOL { FALSE = 0, TRUE = 1 };

typedef struct _Handle Handle;
typedef struct _PtrHandler PtrHandler;

struct _Handle
{
  void*       ptr;
  size_t      size;
  PtrHandler* parent;
};

struct _PtrHandler
{
  Handle**    pointers;
  char**      address;
  enum BOOL*  freed;
  int         length;
  int         pointer_count;
};

extern PtrHandler* ptrhandler;

/* PtrHandler->pointers will be reallocated after this interval
 * Default: 10, meaning after 10 pointers have been allocated PtrHandler->pointers will be reallocated
 * Higher values will speed up the program but will require more buffer memory;
 * Lower value will slow down the program but it is good for programs with little need for dynamic memory
*/
extern int alloc_interval;

/* This value will be added to each allocation done by this library
 * It is 0 by default be I assume that the programmer will provide enough memory for what they need
 * but this is a nice to have just in case (I would suggest an exponent of 2)
*/
extern int alloc_padding;

/* This value is a boolean that will determine is free/alloc _warnings are displayed (for debugging, default 0) */
extern int VERBOSE;

void mptrhandler_init () __attribute__((constructor));
void mptrhandler_deinit () __attribute__((destructor));

void ptrhandler_init (PtrHandler* handler);
void ptrhandler_deinit (PtrHandler* handler);

/* Adds 10 to PtrHandler->pointers if need be */
void ptrhandler_realloc (PtrHandler* handler);

/* Allocates for handles */
Handle* halloc (size_t size);

/* Allocates for handles for custom PtrHandler */
Handle* hhalloc (size_t size, PtrHandler* handler);

/* Allocates for general pointers (creates a handle) */
void* palloc (size_t size);

/* Allocates for general pointers (creates a handle on custom PtrHandler) */
void* phalloc (size_t size, PtrHandler* handler);

/* Get the pointer's handle */
Handle* get_handle (void* ptr);

/* Get the pointer's handle */
Handle* get_hhandle (void* ptr, PtrHandler* handler);

/* Free the handle and the pointer */
void hfree (Handle* handle);

/* Free the pointer and the handle if there is one */
void pfree (void* ptr);

/* Free pointer and handle on custom PtrHandler */
void phfree (void* ptr, PtrHandler* handler);

/* Free all pointers in PtrHandler */
void handler_free (PtrHandler* handler);

/* Check if ptr is valid and free it */
void free_raw (void* ptr);

/* Free an array and all its items */
void array_free (void** ptr);

/* Check if ptr is valid */
int get_valid (void *ptr);

/* Check if handle ptr and handle->ptr are valid */
int get_hvalid (Handle* handle);

/* Printf message is VERBOSE */
void _warn (const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif
