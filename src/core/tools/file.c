/*
 * file.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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


#include "file.h"

int
getlength (char* filename)
{
  int out;
  char *f = NULL;
  systemf ("wc -l %s > temp.t", filename);
  char *buffer = read_file ("temp.t");
  sscanf (buffer, "%d %s", &out, f);
  free (buffer);
  system ("rm -rf temp.t");
  return out;
}

int
get_longest (char* filename)
{
  int out;
  char *f = NULL;
  systemf ("wc -L %s > temp.t", filename);
  char *buffer = read_file ("temp.t");
  sscanf (buffer, "%d %s", &out, f);
  free (buffer);
  system ("rm -rf temp.t");
  
  return out;
}


char** readlines (char* filename)
{
  char **lines = malloc (sizeof(char*) * (getlength(filename)));
  FILE * fp;
  fp = fopen(filename, "r");
  
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  
  if (fp == NULL)
  {
    return NULL;
  }
  
  int curr = 0;
  while ((read = getline(&line, &len, fp)) != -1)
  {
    lines[curr] = mstring_get_sub_py (line, 0, -2);
    if (!get_valid (lines[curr]))
    {
      free_raw (lines[curr]);
      continue;
    }
    curr++;
  }
  
  fclose(fp);
  free (line);
  return lines;
}

char*
read_file (char* filename)
{
  char *buffer = NULL;
  int string_size, read_size;
  FILE *handler = fopen(filename, "r");

  if (handler)
  {
    // Seek the last byte of the file
    fseek(handler, 0, SEEK_END);
    // Offset from the first to the last byte, or in other words, filesize
    string_size = ftell(handler);
    // go back to the start of the file
    rewind(handler);

    // Allocate a string that can hold it all
    buffer = (char*) palloc(sizeof(char) * (string_size + 1) );

    // Read it all in one operation
    read_size = fread(buffer, sizeof(char), string_size, handler);

    // fread doesn't set it so put a \0 in the last position
    // and buffer is now officially a string
    buffer[string_size] = '\0';
    
    if (buffer[string_size-1] == '\n')
    {
      buffer[string_size-1] = ' ';
    }

    if (string_size != read_size)
    {
      // Something went wrong, throw away the memory and set
      // the buffer to NULL
      pfree(buffer);
      buffer = NULL;
    }

    // Always remember to close the file.
    fclose(handler);
  }

  return buffer;
}
