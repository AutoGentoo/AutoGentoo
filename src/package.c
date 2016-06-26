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
  Package      *buff  =  malloc(sizeof(Package*));
  
  buff->type          =  mstring_new   ();
  buff->flags         =  mstring_a_new ();
  buff->category      =  mstring_new   ();
  buff->name          =  mstring_new   ();
  buff->version       =  mstring_new   ();
  buff->slot          =  mstring_new   ();
  buff->repo          =  mstring_new   ();
  buff->vars          =  mstring_new   ();
  buff->file          =  mstring_new   ();
  buff->size          =  0;
  
  return buff;
}

Package *
package_new_from_string (mstring str)
{
  //[ebuild   R    ] sys-devel/binutils-2.25.1-r1:2.25.1::gentoo  USE="cxx nls zlib -multitarget -static-libs {-test} -vanilla (-multislot%)" 0 KiB
  

int               package_do_stage           (Package*, mstring);
