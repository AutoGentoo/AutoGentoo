/*
 * configparser.c
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


#include "configparser.h"

Variable
variable_new (void)
{
  Variable       buff   =  malloc (sizeof(Variable);
  buff.name             =  mstring_new ();
  buff.value            =  mstring_new ();
  
  return buff;
}

Variable
variable_new_from_str (mstring in)
{
  Variable       buff   =  variable_new ();
  buff.name             =  mstring_find_before (in, '=');
  buff.value            =  mstring_find_after (in, '=');
  
  return buff;
}

ArgVariable
argvariable_new (void)
{
  ArgVariable    buff   =  malloc (sizeof(ArgVariable);
  buff.name             =  mstring_new ();
  buff.value            =  mstring_new ();
  buff.arguments        =  mstring_a_new ();
  buff.full_value       =  mstring_new ();
  buff.argc             =  0;
  
  return buff;
}

ArgVariable
argvariable_new_from_str (mstring in)
{
  ArgVariable    buff   =  argvariable_new ();
  buff.name             =  mstring_find_before (in, '=');
  buff.full_value       =  mstring_find_after (in, '=');
}

ListVariable
listvariable_new (void)
{
  ListVariable   buff   =  malloc (sizeof(ListVariable);
  buff.name             =  mstring_new ();
  
  return buff;
}

ListVariable      listvariable_new_from_str       (mstring);

Section           section_new                     (void);

Section           section_new_from_str            (mstring);
