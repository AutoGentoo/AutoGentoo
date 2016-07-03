/*
 * package.c
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
#include "package.h"

Package *
package_new (void)
{
  Package      *buff  =  malloc           (sizeof(Package*));
  
  buff->type          =  mstring_new      ();
  buff->flags         =  pkgproperty_new  ();
  buff->category      =  mstring_new      ();
  buff->name          =  mstring_new      ();
  buff->version       =  pkgversion_new   ();
  buff->slot          =  mstring_new      ();
  buff->repo          =  mstring_new      ();
  buff->vars          =  mstring_new      ();
  buff->file          =  mstring_new      ();
  buff->size          =  0;
  
  return buff;
}

PkgVersion  *
pkgversion_new (void)
{
  PkgVersion    *buff  =  malloc(sizeof(PkgVersion*));
  buff->revision       =  mstring_new ();
  buff->version        =  mstring_new ();
  
  return buff;
}

PkgProperty *
pkgproperty_new (void)
{
  PkgProperty   *buff  =  malloc(sizeof(PkgProperty*));
  buff->new            =  FALSE;
  buff->slot           =  FALSE;
  buff->updating       =  FALSE;
  buff->downgrading    =  FALSE;
  buff->reinstall      =  FALSE;
  buff->replacing      =  FALSE;
  buff->fetch_man      =  FALSE;
  buff->fetch_auto     =  FALSE;
  buff->interactive    =  FALSE;
  buff->blocked_man    =  FALSE;
  buff->blocked_auto   =  FALSE;
  
  return buff;
}

Package *
package_new_from_string (mstring str)
{
  Package     *buff       = package_new ();
  //[ebuild   R    ] sys-devel/binutils-2.25.1-r1:2.25.1::gentoo  USE="cxx nls zlib -multitarget -static-libs {-test} -vanilla (-multislot%)" 0 KiB
  
  int          curr       = 0;
  PkgProperty *flag_buff;
  mstring      flag_buff_str;
  mstring      first_buff = mstring_find_before (str, ']');
  mstring      second_buff= mstring_find_after (str, ']');
  int          name_dev   = mstring_find (second_buff, '/');
  
  first_buff              = mstring_get_sub_py (first_buff, 1, mstring_get_length (first_buff) - 1);
  
  buff->type              = mstring_find_before (first_buff, ' ');
  flag_buff_str           = mstring_find_after (first_buff, ' '); 
  flag_buff               = pkgproperty_new ();
  
  for (;curr != mstring_get_length (flag_buff_str); curr++)
  {
    if (flag_buff_str [curr] == ' ')
    {
      continue;
    }
    switch (flag_buff_str [curr])
    {
      case 'N':
      flag_buff->new = TRUE;
      case 'S':
      flag_buff->slot = TRUE;
      case 'U':
      flag_buff->updating = TRUE;
      case 'D':
      flag_buff->downgrading = TRUE;
      case 'r':
      flag_buff->reinstall = TRUE;
      case 'R':
      flag_buff->replacing = TRUE;
      case 'F':
      flag_buff->fetch_man = TRUE;
      case 'f':
      flag_buff->fetch_auto = TRUE;
      case 'I':
      flag_buff->interactive = TRUE;
      case 'B':
      flag_buff->blocked_man = TRUE;
      case 'b':
      flag_buff->blocked_auto = TRUE;
    }
  }
  
  buff->flags = flag_buff;
  buff->category = mstring_get_sub_py (second_buff, 1, name_dev);
  
  mstring name_buff       = mstring_get_sub_py (second_buff, name_dev, mstring_find (second_buff, ' '));
  
  
  return buff;
}

//int               package_do_stage           (Package*, mstring);
