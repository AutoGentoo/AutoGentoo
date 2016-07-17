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
  Package      *buff  =  (Package*)malloc(sizeof(Package));
  buff->type          =  mstring_new      ();
  buff->flags         =  pkgproperty_new  ();
  buff->category      =  mstring_new      ();
  buff->name          =  mstring_new      ();
  buff->version       =  pkgversion_new   ();
  buff->old           =  pkgversion_new   ();
  buff->parameters    =  map_new          ();
  buff->file          =  mstring_new      ();
  buff->name_raw      =  mstring_new      ();
  buff->size          =  mstring_new      ();
  
  return buff;
}

PkgVersion  *
pkgversion_new (void)
{
  PkgVersion    *buff  =  (PkgVersion*)malloc(sizeof(PkgVersion));
  buff->revision       =  mstring_new ();
  buff->version        =  mstring_new ();
  buff->repo           =  mstring_new ();
  buff->full           =  mstring_new ();
  buff->slot           =  mstring_new ();
  
  return buff;
}

void
pkgversion_parse (PkgVersion* buff, mstring in)
{
  buff->full     =  in;
  buff->repo     =  mstring_get_sub_py (buff->full, mstring_search (buff->full, "::") + 2, -1);
  mstring     vb =  mstring_get_sub_py (buff->full, 0, mstring_search (buff->full, "::"));
  buff->version  =  vb;
  
  if (buff->full [mstring_rfind (buff->version, '-')+1] == 'r')
  {
    buff->revision   =  mstring_get_sub_py (vb, mstring_rfind (vb, '-')+1, -1);
  }
  
  int slot_splt = mstring_find (vb, ':');
  if (slot_splt != -1)
  {
    buff->slot = mstring_get_sub_py (vb, slot_splt + 1, -1);
  }
  else
  {
    buff->slot = "0";
  }
}

PkgVersion  *
pkgversion_new_from_str (mstring in)
{
  PkgVersion  *buff  =  pkgversion_new ();
  pkgversion_parse                     (buff, in);
  
  return buff;
}

PkgProperty *
pkgproperty_new (void)
{
  PkgProperty   *buff  =  (PkgProperty*)malloc(sizeof(PkgProperty));
  pkgproperty_reset       (buff);
  
  return buff;
}

void
pkgproperty_reset (PkgProperty* buff)
{
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
}

Package *
package_new_from_string (mstring str)
{
  Package     *buff       = package_new ();
  
  int          curr       = 0;
  PkgProperty *flag_buff = pkgproperty_new ();
  mstring      flag_buff_str;
  mstring      first_buff = mstring_find_before (str, ']');
  
  mstring      second_buff= mstring_get_sub_py (mstring_find_after (mstring_find_after (str, ']'), ' '), 1, -1);
  
  int          name_dev   = mstring_find (second_buff, '/');
  
  first_buff              = mstring_get_sub_py (first_buff, 1, -1);
  buff->type              = mstring_find_before (first_buff, ' ');
  flag_buff_str           = mstring_find_after (first_buff, ' '); 
  pkgproperty_reset (flag_buff);
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
        break;
      case 'S':
        flag_buff->slot = TRUE;
        break;
      case 'U':
        flag_buff->updating = TRUE;
        break;
      case 'D':
        flag_buff->downgrading = TRUE;
        break;
      case 'r':
        flag_buff->reinstall = TRUE;
        break;
      case 'R':
        flag_buff->replacing = TRUE;
        break;
      case 'F':
        flag_buff->fetch_man = TRUE;
        break;
      case 'f':
        flag_buff->fetch_auto = TRUE;
        break;
      case 'I':
        flag_buff->interactive = TRUE;
        break;
      case 'B':
        flag_buff->blocked_man = TRUE;
        break;
      case 'b':
        flag_buff->blocked_auto = TRUE;
        break;
    }
  }
  buff->flags = flag_buff;
  buff->category = mstring_get_sub_py (second_buff, 0, name_dev);
  
  buff->name_raw = mstring_get_sub_py (second_buff, name_dev + 1, mstring_find (second_buff, ' '));
  
  int splt = 0;
  if (buff->name_raw [mstring_rfind (buff->name_raw, '-')+1] == 'r')
  {
    mstring nr_b = mstring_get_sub_py (buff->name_raw, 0, mstring_rfind (buff->name_raw, '-'));
    splt = mstring_rfind (nr_b, '-');
  }
  else
  {
    splt = mstring_rfind (buff->name_raw, '-');
  }
  buff->name     = mstring_get_sub_py (buff->name_raw, 0, splt);
  buff->version  = pkgversion_new_from_str (mstring_get_sub_py (buff->name_raw, splt+1, -1));
  
  mstring b_vars;
  
  if (mstring_find (second_buff, ']') == -1)
  {
    b_vars = mstring_get_sub_py (second_buff, mstring_find (second_buff, ' ') + 2, mstring_rfind (mstring_get_sub_py (second_buff, 0, mstring_rfind (second_buff, ' ')), ' '));
    buff->old      = NULL;
  }
  else
  {
    b_vars = mstring_get_sub_py (second_buff, mstring_find (second_buff, ']') + 2, mstring_rfind (mstring_get_sub_py (second_buff, 0, mstring_rfind (second_buff, ' ')), ' '));
    buff->old      = pkgversion_new_from_str (mstring_get_sub_py (second_buff, mstring_find (second_buff, '[') + 1, mstring_find (second_buff, ']')));
  }
  
  mstring_a b_var_a = mstring_split_quote (b_vars, ' ');
  for (curr = 0; curr != mstring_split_quote_len (b_vars, ' '); curr++)
  {
    mstring_a splt_b = mstring_split (b_var_a [curr], '=');
    map_add (buff->parameters, splt_b [0], mstring_grate (splt_b[1]));
  }
  
  buff->size = mstring_new ();
  buff->size = mstring_get_sub_py (str, mstring_rfind (mstring_get_sub_py (str, 0, mstring_rfind (str, ' ')), ' ') + 1, mstring_rfind (str, ' '));
  
  buff->file = mstring_new ();
  sprintf (buff->file, "/usr/portage/%s/%s/%s-%s.ebuild", buff->category, buff->name, buff->name, buff->version->version);
  
  free (b_var_a);
  free (b_vars);
  free (flag_buff_str);
  free (first_buff);
  free (second_buff);
  return buff;
}

int
package_do_stage (Package* pkg, mstring stage)
{
  mstring buff = mstring_new ();
  sprintf (buff, "ebuild %s %s", pkg->file, stage);
  
  return system (buff);
}
