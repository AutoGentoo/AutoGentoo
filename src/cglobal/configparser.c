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
  Variable       buff = {
    .name             = mstring_new (),
    .value            = mstring_new (),
    .arguments        = mstring_a_new (),
    .full_value       = mstring_new ()
  };
  
  return buff;
}

Variable
variable_new_from_str (mstring in)
{
  Variable        buff    =  variable_new ();
  buff.name               =  mstring_find_before (in, '=');
  buff.full_value         =  mstring_find_after (in, '=');
  int             get_eq  =  mstring_find (in, '=');
  int             get_arg =  mstring_find_start (in, ' ', get_eq);
  buff.value              =  mstring_get_sub_py (in, get_eq, get_arg);
  buff.arguments          =  mstring_split (mstring_get_sub_py (in, get_arg, -1), ' ');
  
  return buff;
}

Section
section_new (void)
{
  Section       buff  = {
    .name             = mstring_new (),
    .variables        = malloc(sizeof(Variable) * 2048),
    .varc             = 0,
    .comments         = malloc(sizeof(mstring_a) * 1024)
  };
  
  return buff;
}

Section
section_new_from_str (mstring_a in)
{
  Section       buff = section_new ();
  buff.name          = mstring_grate (in[0]);
  
  int curr;
  int curr_comment = 0;
  for (curr = 1; in [curr]; ++curr)
  {
    if (in[curr][0] == '#')
    {
      buff.comments[curr_comment] = in[curr];
      curr_comment++;
      continue;
    }
    
    if (!in[curr])
      break;
    
    Variable currVar = variable_new_from_str (in[curr]);
    buff.variables[buff.varc] = currVar;
    buff.varc++;
  }
  
  return buff;
}

Config
config_new (void)
{
  Config       buff   = {
    .file             = mstring_new (),
    .comments         = malloc(sizeof(mstring_a) * 1024),
    .sections         = malloc(sizeof(Section) * 1024),
    .sectionc         = 0
  };
  
  return buff;
}

Config
config_new_from_str (mstring filename)
{
  Config       buff  = config_new ();
  buff.file          = filename;
  mstring_a file     = readlines (filename);
  
  int curr = 0;
  int curr_comment = 0;
  bool in_section = FALSE;
  mstring_a buff_section = malloc(sizeof(mstring_a) * 3072);
  int buff_s = 0;
  for (; file[curr]; curr++)
  {
    if (file[curr][0] == '#')
    {
      buff.comments[curr_comment] = file[curr];
      curr_comment++;
      continue;
    }
    
    if (in_section)
    {
      if (strcmp (file[curr], "\n") == 0)
      {
        Section sec_buff = section_new_from_str (buff_section);
        buff.sections[buff.sectionc] = sec_buff;
        buff.sectionc++;
        in_section = FALSE;
        buff_s = 0;
        free(buff_section);
        continue;
      }
      buff_section[buff_s] = file[curr];
      buff_s++;
      continue;
    }
    
    if (file[curr][0] == '[')
    {
      in_section = TRUE;
      buff_section[buff_s] = file[curr];
      buff_s++;
    }
  }
  
  free(file);
  
  return buff;
}

void
config_free (Config nptr)
{
  free (nptr.file);
  free (nptr.comments);
  free (nptr.sections);
}
