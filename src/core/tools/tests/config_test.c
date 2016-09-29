/*
 * config_test.c
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


#include <stdio.h>
#include "mstring.h"
#include "iniparser.h"

int main(int argc, char **argv)
{
  dictionary  *   ini ;
  
  char* ini_name = "../../autogentoo.conf";
  
  ini = iniparser_load(ini_name);
  
  if (ini==NULL)
  {
    fprintf(stderr, "cannot parse file: %s\n", ini_name);
    return -1 ;
  }
  
  printf ("%s\n", iniparser_getstring(ini, "user:fullname", NULL));
  
  return 0;
}
