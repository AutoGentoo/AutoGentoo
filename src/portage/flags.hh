/*
 * flags.h
 * 
 * Copyright 2016 Unknown <atuser@Kronos>
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

#ifndef __AUTOGENTOO_FLAG_HH__
#define __AUTOGENTOO_FLAG_HH__

#include <iostream>
#include <_misc_tools.hh>

using namespace std;

typedef struct
{
  char* str;
  bool value;
} Flag;

typedef struct
{
  char* type;
} Opt;

typedef struct
{
  void parse (string, string);
  void content (string);
  void str (string);
  
  bool is_flag;
  
  Flag flag;
  Opt opt;
  
  char* _content;
} Arg;

int        get_close            (string paren);

void       parseblock           (string blockRaw);

#endif
