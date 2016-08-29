/*
 * ptr_handler.c
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


#include <stdio.h>
#include <ptr_handler.h>

PointerHandler* handler = NULL;
int POINTER_SIZE = 0;
int VERBOSE = 0;
int PALLOC_ADD = 4;

void handler_init ()
{
  handler = malloc (sizeof(PointerHandler) + 1);
  handler->count = 1;
  handler->ptrs = malloc (sizeof(void*) * handler->count);
}

void* palloc (size_t size)
{
  /* Allocate space for the pointer */
  handler->ptrs = realloc (handler->ptrs, sizeof(void*) * handler->count + PALLOC_ADD);
  //~ handler->freed = realloc (handler->freed, sizeof(void*) * handler->freed_count + PALLOC_ADD);
  
  /* Create the pointer */
  void* ptr = malloc (size + PALLOC_ADD);
  
  if (VERBOSE)
  {
    printf ("Allocating pointer at %p\n", ptr);
  }
  
  if (!get_valid (ptr))
  {
    pfree (ptr);
    return NULL;
  }
  
  handler->ptrs[handler->count-1] = ptr;
  handler->count++;
  //~ handler->freed_count++;
  
  return ptr;
}

void pfree (void* ptr)
{
  if (ptr == NULL)
  {
    return;
  }
  if (!get_valid (ptr))
  {
    if (VERBOSE)
    {
      printf ("error: pointer %p is not valid!\n", ptr);
    }
    return;
  }
  
  if (strcmp ((char*)ptr, "") == 0)
  {
    return;
  }
  
  if (VERBOSE)
  {
    printf ("Deallocating pointer at %p (%s)\n", ptr, (char*)ptr);
  }
  
  free (ptr);
}

void array_free_bare (void **ptr)
{
  int i;
  for (i=0; ptr[i]; i++)
  {
    free (ptr[i]);
  }
  
  free (ptr);
}

void array_free (void **ptr)
{
  int i;
  for (i=0; ptr[i]; i++)
  {
    pfree (ptr[i]);
  }
  
  pfree (ptr);
}

/*void set_valid (void *ptr)
{
  char sbuff[25];
  snprintf (sbuff, 25, "%p", ptr);
  
  for (;sbuff[POINTER_SIZE]; POINTER_SIZE++);
  
  * Check if pointer is valid *
  if (POINTER_SIZE < 8 || POINTER_SIZE > 10)
  {
    printf ("Pointer %p sent to void set_valid (void *ptr) is not valid!\n", ptr);
    raise(SIGSEGV);
  }
}*/

int get_valid (void *ptr)
{
  if (!ptr)
  {
    return 0;
  }
  char sbuff[25];
  snprintf (sbuff, 25, "%p", ptr);
  
  int size = 0;
  for (;sbuff[size]; size++);
  
  if (size != 8 && size != 9 && size != 10)
  {
    return 0;
  }
  return 1;
}

void free_all ()
{
  int i;
  for (i=0; i != handler->count; i++)
  {
    pfree (handler->ptrs[i]);
  }
  free (handler->ptrs);
  
  pfree (handler);
}
