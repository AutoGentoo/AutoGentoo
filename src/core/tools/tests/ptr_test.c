/*
 * ptr_test.c
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

int main(int argc, char **argv)
{
  VERBOSE = 1;
  char* test = palloc (20 * sizeof(char));
  sprintf (test, "Hello World");
  
  char** _test = palloc (1 * sizeof(char*));
  _test[0] = palloc (6 * sizeof(char));
  
  sprintf (_test[0], "Hello");
  
  printf ("%s\n", test);
  printf ("%s\n", _test[0]);
  return 0;
}

