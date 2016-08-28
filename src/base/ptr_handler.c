/*
 * untitled.c
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
#include <base/ptr_handler.h>

PointerHandler* mainHandler = NULL;
int POINTER_SIZE = 0;
int VERBOSE = 0;

void main_init ()
{
  handler_init (malloc (sizeof(PointerHandler)));
}

int handler_init (PointerHandler* handler)
{
  mainHandler = handler;
  
  int* mbuff = malloc (sizeof(int));
  if (mbuff == NULL)
  {
    printf ("Program ran into an error while initializing!\n");
    raise (SIGSEGV);
  }
  
  set_valid (mbuff);
  free (mbuff);
  
  /* 
   * Alloc space for the main group
  */
  mainHandler->size = 1;
  mainHandler->groups = malloc (sizeof(PointerGroup*) * mainHandler->size);
  
  mainHandler->groups[0] = malloc (sizeof(PointerGroup*));
  mainHandler->groups[0]->size = 0;
  mainHandler->groups[0]->ptrs = malloc (sizeof(void*) * mainHandler->groups[0]->size);
  mainHandler->groups[0]->is_array = malloc (sizeof(int) * mainHandler->groups[0]->size);
  
  if (!get_valid (mainHandler->groups))
  {
    return 1;
  }
  return 0;
}

PointerGroup* new_group ()
{
  mainHandler->size++;
  mainHandler->groups = realloc (mainHandler->groups, sizeof(PointerGroup*) * mainHandler->size);
  mainHandler->groups[mainHandler->size-1] = malloc (sizeof(PointerGroup*));
  
  mainHandler->groups[mainHandler->size-1]->size = 1;
  mainHandler->groups[mainHandler->size-1]->ptrs = malloc (sizeof(void*) * mainHandler->groups[mainHandler->size-1]->size);
  
  return mainHandler->groups[mainHandler->size-1];
}

void* paalloc (size_t size, size_t tsize)
{
  return gaalloc (size, tsize, mainHandler->groups[0]);
}

void* palloc (size_t size, size_t tsize)
{
  return galloc (size, tsize, mainHandler->groups[0]);
}

void* gaalloc (size_t size, size_t tsize, PointerGroup* group)
{
  void* ptr = galloc (size, tsize, group);
  group->is_array[group->size-1] = 1;
  return ptr;
}

void* galloc (size_t size, size_t tsize, PointerGroup* group)
{
  /* Allocate space for the next pointer */
  group->size++;
  group->ptrs = realloc (group->ptrs, sizeof(PointerGroup*) * group->size++);
  
  void* ptr = malloc (size);
  if (!get_valid (ptr))
  {
    return NULL;
  }
  
  group->ptrs [group->size-1] = ptr;
  group->is_array[group->size-1] = 0;
  
  return ptr;
}

void pfree (void *ptr)
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
  
  return free (ptr);
}

void ar_pfree (void **ar_ptr)
{
  int i;
  for (i=0; ar_ptr[i]; i++)
  {
    pfree (ar_ptr[i]);
  }
  pfree (ar_ptr);
}

void gfree (PointerGroup* pgrp)
{
  int i;
  for (i=0; i != pgrp->size; i++)
  {
    if (pgrp->is_array[i])
    {
      ar_pfree (pgrp->ptrs[i]);
      continue;
    }
    pfree (pgrp->ptrs[i]);
  }
  pfree (pgrp->ptrs);
  pfree (pgrp->is_array);
  pfree (pgrp);
}

void set_valid (void *ptr)
{
  char sbuff[25];
  snprintf (sbuff, 25, "%p", ptr);
  
  for (;sbuff[POINTER_SIZE]; POINTER_SIZE++);
  
  /* Check if pointer is valid */
  if (POINTER_SIZE < 8 || POINTER_SIZE > 10)
  {
    printf ("Pointer %p sent to void set_valid (void *ptr) is not valid!\n", ptr);
    raise(SIGSEGV);
  }
}

int get_valid (void *ptr)
{
  char sbuff[25];
  snprintf (sbuff, 25, "%p", ptr);
  
  int size = 0;
  for (;sbuff[size]; size++);
  
  if (size != POINTER_SIZE)
  {
    return 0;
  }
  return 1;
}

void free_all ()
{
  int i;
  for (i=0; i != mainHandler->size; i++)
  {
    gfree (mainHandler->groups[i]);
  }
  pfree (mainHandler->groups);
  pfree (mainHandler);
}
