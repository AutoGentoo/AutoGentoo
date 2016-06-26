/*
 * mlib.h
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
#include <stdlib.h>
#include <string.h>

#ifndef   AUTOGENTOO_BOOL
#define   AUTOGENTOO_BOOL
typedef   int     bool;
#endif

#ifndef   NULL
#define   NULL    ((void*) 0)
#endif

#ifndef   FALSE
#define   FALSE   (0)
#endif

#ifndef   TRUE
#define   TRUE    (!FALSE)
#endif

#ifndef   MSTRING
#define   MSTRING
typedef char     mchar;
typedef mchar   *mstring;
typedef mchar  **mstring_a;
typedef int     *int_a;
#endif

#ifndef   AUTOGENTOO_CXXTYPES
#define   AUTOGENTOO_CXXTYPES
mstring itoa(int val, int base)
{
  static mchar buf[32] = {0};
  
  int i = 30;
  
  for(; val && i ; --i, val /= base)
  {
    buf[i] = "0123456789abcdef"[val % base];
  }
  
  return &buf[i+1];
  
}

#endif
