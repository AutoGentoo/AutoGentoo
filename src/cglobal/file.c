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
getlength (mstring filename)
{
  int out;
  char *f = NULL;
  system (rprintf ("wc -l %s > temp.t\n", filename));
  sscanf (read_file ("temp.t"), "%d %s", &out, f);
  system ("rm -rf temp.t");
  
  return out;
}

int
get_longest (mstring filename)
{
  int out;
  char *f = NULL;
  system (rprintf ("wc -L %s > temp.t\n", filename));
  sscanf (read_file ("temp.t"), "%d %s", &out, f);
  system ("rm -rf temp.t");
  
  return out;
}


void readlines (char** lines, mstring b_filename)
{
  FILE * fp;
  fp = fopen(b_filename, "r");
  //char lines[getlength(b_filename) + 1][get_longest (b_filename)];
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  
  if (fp == NULL)
  {
    return;
  }
  
  int curr = 0;
  
  while ((read = getline(&line, &len, fp)) != -1)
  {
    lines[curr] = malloc (sizeof(char) * mstring_get_length (line) - 1);
    memcpy (lines[curr], line, mstring_get_length(line) - 1);
    curr++;
  }
  
  fclose(fp);
  if (line)
  {
    free(line);
  }
  
  return;
}

mstring
read_file (mstring filename)
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
    buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

    // Read it all in one operation
    read_size = fread(buffer, sizeof(char), string_size, handler);

    // fread doesn't set it so put a \0 in the last position
    // and buffer is now officially a string
    buffer[string_size] = '\0';

    if (string_size != read_size)
    {
      // Something went wrong, throw away the memory and set
      // the buffer to NULL
      free(buffer);
      buffer = NULL;
    }

    // Always remember to close the file.
    fclose(handler);
  }

  return buffer;
}
