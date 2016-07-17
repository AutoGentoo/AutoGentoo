/*
 * map.c
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
#include "map.h"

map *
map_new (void)
{
  map        *buff  =  (map*)malloc  (sizeof(map));
  buff->variables   =  mstring_a_new ();
  buff->values      =  mstring_a_new ();
  buff->var_num     =  0;
  
  return buff;
}

void
map_add (map* to_change, mstring var, mstring val)
{
  to_change->variables [to_change->var_num] = var;
  to_change->values [to_change->var_num] = val;
  to_change->var_num++;
};

mstring
map_get (map* from, mstring var)
{
  int buff = mstring_a_find (from->variables, var);
  if (buff == -1) 
  {
    return NULL;
  }
  return from->values [buff];
}
