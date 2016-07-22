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
  int lines;
  int ch=0;
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL)
    err(1, "%s", filename);
  
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  
  return lines;
}

mstring_a
readlines (mstring filename)
{
  FILE *f;
  size_t len;
  char *line;
  mstring_a out = malloc (sizeof(mstring_a) * getlength (filename));
  int curr = 0;
  
  f = fopen(filename, "r");
  if (f == NULL)
    err(1, "%s", filename);
  
  while ((line = fgetln(f, &len)))
  {
    /** Create segmentation fault because of miss allocated memory **/
    /*mstring b_line = malloc(sizeof(mchar) * strlen(line)+1);
    strcpy(b_line, line);
    removeChar(b_line, '\n');
    out[curr] = malloc(sizeof(mchar) * strlen(b_line));
    out[curr] = b_line;
    free(b_line);*/
    curr++;
  }
  if (!feof(f))
    err(1, "fgetln");
  
  free(line);
  
  fclose(f);
  
  return out;
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
