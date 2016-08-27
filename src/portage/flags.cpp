/*
 * flags.c
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


#include <stdio.h>
#include <flags.hh>

int get_close (string paren)
{
  int curr_p;
  int i;
  
  for (curr_p = 0, i = 0; paren[i]; i++)
  {
    if (paren[i] == '(')
    {
      curr_p++;
    }
    if (paren[i] == ')')
    {
      curr_p--;
    }
    if (curr_p == 0)
    {
      break;
    }
  }
  return i;
}


void parseblock (string blockRaw)
{
  string::iterator buff_i = blockRaw.begin (); 
  string buff;
  int old = -1;
  int it;
  
  bool pa = false;
  
  int par = 0;
  int b_s = 0;
  int b_e = 0;
  int curr_arg = 0;
  string p_buff;
  
  Arg* out = new Arg [128];
  
  for (it = 0; blockRaw.c_str ()[it]; ++it)
  {
    if (blockRaw.at (it) == '(')
    {
      if (par == 0)
      {
        b_s = it;
      }
      par++;
      pa = true;
    }
    
    if (blockRaw.at (it) == ')')
    {
      par--;
      if (par == 0)
      {
        b_e = it;
        if (pa)
        {
          p_buff =  misc::substr (blockRaw, b_s + 2, b_e-1);
          out[curr_arg].content (p_buff);
        }
      }
    }
    
    if (par != 0)
    {
      continue;
    }
    
    if (blockRaw.at(it) == ' ' && blockRaw.at(it-1) == '?')
    {
      buff = misc::substr (blockRaw, old+1, it);
      cout << buff << endl;
      old = it;
      continue;
    }
    
    if (blockRaw.at(it) == ' ')
    {
      old = it;
    }
  }
}

void Arg::parse (string arg, string val)
{
  if (arg[arg.length ()] == '?')
  {
    is_flag = true;
  }
  else
  {
    is_flag = false;
  }
  
}

void Arg::content (string in)
{
  _content = (char*) in.c_str ();
}
