/*
 * stage3.c
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


#include <core/stage3.h>

Stage3
stage3_new (char* arch)
{
  Stage3       buff;
  buff.arch = arch;
  sprintf(buff.buff_link, "http://distfiles.gentoo.org/releases/%s/autobuilds/latest-stage3.txt", buff.arch);
  buff.name = malloc (sizeof(char) * 45);
  buff.date = malloc (sizeof(char) * 10);
  
  return buff;
}

Stage3
stage3_new_latest (char* arch)
{
  Stage3 out = stage3_new (arch);
  download_to_file (out.buff_link, "stage3_buff.txt");
  char ** filelines = readlines ("stage3_buff.txt");
  char *  mainline = mstring_find_before (file[2], ' ');
  out.date = mstring_find_before (file[2], '/');
  out.name = mstring_get_sub_py (mainline, mstring_get_length (out.date) + 1, -1);
  
  return out;
}

