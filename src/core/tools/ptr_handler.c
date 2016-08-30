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

PtrHandler* ptrhandler = NULL;
int alloc_interval = 10;
int alloc_padding = 0;
int VERBOSE = 0;

void mptrhandler_init ()
{
  ptrhandler = malloc (sizeof(PtrHandler));
  ptrhandler_init (ptrhandler);
}

void mptrhandler_deinit ()
{
  handler_free (ptrhandler);
}

void ptrhandler_init (PtrHandler* handler)
{
  handler->length = 5;
  handler->pointer_count = 0;
  handler->pointers = malloc (sizeof (Handle*) * handler->length);
  handler->address = malloc (sizeof (char*) * handler->length);
  handler->freed = malloc (sizeof (enum BOOL) * handler->length);
  ptrhandler_realloc (handler);
}

void ptrhandler_deinit (PtrHandler* handler)
{
  handler_free (handler);
}

void ptrhandler_realloc (PtrHandler* handler)
{
  if (handler->pointer_count >= handler->length)
  {
    handler->length += alloc_interval;
    handler->pointers = realloc (handler->pointers, sizeof (Handle*) * handler->length);
    handler->address = realloc (handler->address, sizeof (char*) * handler->length);
    handler->freed = realloc (handler->freed, sizeof (enum BOOL) * handler->length);
  }
}

Handle* halloc (size_t size)
{
  return hhalloc (size, ptrhandler);
}

Handle* hhalloc (size_t size, PtrHandler* handler)
{
  handler->pointer_count++;
  ptrhandler_realloc (handler);
  
  handler->pointers[handler->pointer_count-1] = malloc (sizeof(Handle*) + 8);
  handler->pointers[handler->pointer_count-1]->size = size;
  handler->pointers[handler->pointer_count-1]->parent = handler;
  handler->freed[handler->pointer_count-1] = FALSE;
  
  handler->pointers[handler->pointer_count-1]->ptr = malloc (size+alloc_padding);
  handler->address[handler->pointer_count-1] = malloc (sizeof(char) * 20);
  sprintf (handler->address[handler->pointer_count-1], "%p", handler->pointers[handler->pointer_count-1]->ptr);
  _warn ("Allocated pointer at %p with handle at %p", handler->pointers[handler->pointer_count-1]->ptr, handler->pointers[handler->pointer_count-1]);
  
  return handler->pointers[handler->pointer_count-1];
}

void* palloc (size_t size)
{
  return halloc (size)->ptr;
}

void* phalloc (size_t size, PtrHandler* handler)
{
  return hhalloc (size, handler)->ptr;
}

Handle* get_handle (void* ptr)
{
  return get_hhandle (ptr, ptrhandler);
}

int get_hhandle_i (Handle* handle, PtrHandler* handler)
{
  int i;
  for (i=0; i != handler->pointer_count; i++)
  {
    Handle* buff = handler->pointers[i];
    if (buff == handle)
    {
      return i;
    }
  }
  
  return -1;
}

Handle* get_hhandle (void* ptr, PtrHandler* handler)
{
  int i;
  for (i=0; i != handler->pointer_count; i++)
  {
    Handle* buff = handler->pointers[i];
    if (buff->ptr == ptr)
    {
      return buff;
    }
  }
  _warn ("Handler not found for address %p", ptr);
  return NULL;
}

void hfree (Handle* handle)
{
  if (!get_hvalid (handle))
  {
    return;
  }
  
  if (handle->parent->freed[get_hhandle_i (handle, handle->parent)] == TRUE)
  {
    return;
  }
  
  handle->parent->freed[get_hhandle_i (handle, handle->parent)] = TRUE;
  
  free_raw (handle->ptr);
  free_raw (handle);
}

void pfree (void* ptr)
{
  Handle* handle = get_handle (ptr);
  if (handle == NULL)
  {
    return free_raw (ptr);
  }
  
  hfree (handle);
}

void phfree (void* ptr, PtrHandler* handler)
{
  hfree (get_hhandle (ptr, handler));
}

void handler_free (PtrHandler* handler)
{
  int i;
  for (i=0; i != handler->pointer_count; i++)
  {
    hfree (handler->pointers[i]);
    free_raw (handler->address[i]);
  }
  free_raw (handler->pointers);
  free_raw (handler->address);
  free_raw (handler->freed);
  free_raw (handler);
}

void free_raw (void* ptr)
{
  if (get_valid (ptr))
  {
    _warn ("Deallocating pointer with address %p", ptr);
    free (ptr);
    return;
  }
  _warn ("Pointer at address %p is not valid", ptr);
}

void array_free (void** ptr)
{
  int i;
  for (i=0; ptr[i]; i++)
  {
    free_raw (ptr[i]);
  }
  
  free_raw (ptr);
}

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

int get_hvalid (Handle* handle)
{
  if (!get_valid (handle) || !get_valid (handle->ptr))
  {
    return 0;
  }
  return 1;
}

void _warn (const char* format, ...)
{
  if (!format || !VERBOSE)
  {
    return;
  }
  
  char       msg[1024];
  va_list    args;

  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args); // do check return value
  va_end(args);

  printf ("%s\n", msg);
}
