/*
 * mstring.c
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


#include "mstring.h"

/* Create a new empty string */
mstring
mstring_new (void)
{
  return malloc(sizeof(mstring));
}

/* Create a new empty string */
mstring
mstring_new_from_char (mchar c)
{
  mstring  buff;
  
  buff = &c;
  
  return buff;
}

mstring
mstring_new_from_chars (char *c)
{
  return c;
}

int
mstring_get_length (mstring str)
{
  if (!str)
  {
    return 0;
  }
  
  int num;
  for (num = 0; str[num]; num++);
  return num;
}

mstring
mstring_get_sub  (mstring old, int start, int length)
{
  int      size = mstring_get_length (old);
  mchar    out_array [length-start];
  
  if (size < (start+length))
  {
    length = size-start;
  }
  
  int curr;
  
  for (curr = 0; curr != length; curr++)
  {
    out_array [curr] = old[curr+start];
  }
  
  return mstring_new_from_chars (&out_array[0]);
}

mstring
mstring_get_sub_py (mstring old, int start, int end)
{
  int      size = mstring_get_length (old);
  int      len;
  mstring  out_array = (char*) malloc(size);
  
  if (size < end || end == -1)
  {
    end = size;
  }
  len = end - start;
  memcpy(out_array, &old[start], len);
  
  return mstring_new_from_chars (out_array);
}

int
mstring_find (mstring in, mchar find)
{
  int   size = mstring_get_length (in);
  int   curr;
  
  for (curr = 0; curr != size; curr++)
  {
    char c = in[curr];
    
    if (c == find)
    {
      return curr;
    }
  }
  
  return -1;
}

int
mstring_rfind (mstring in, char find)
{
  int   size = mstring_get_length (in);
  int   curr = size;
  
  for (; curr != 0; curr--)
  {
    char c = in[curr];
    
    if (c == find)
    {
      return curr;
    }
  }
  
  return -1;
}

int
mstring_find_start (mstring in, mchar find, int start)
{
  int   size = mstring_get_length (in);
  int   curr;
  char  buff [size];
  
  strcpy(buff, in);
  
  for (curr = start; curr != size; curr++)
  {
    char c = buff[curr];
    
    if (c == find)
    {
      return curr;
    }
  }
  
  return -1;
}

/*char*
char_a_new_from_string (mstring in)
{
  int     size    = mstring_get_length (in);
  char    buff[size];
  int     curr    = 0;
  
  for (;in[curr]; curr++)
  {
    buff [curr] = in[curr];
  }
  
  return buff;
}*/

mstring
mstring_find_before  (mstring old, mchar c)
{
  return mstring_get_sub_py (old, 0, mstring_find (old, c));
}

mstring
mstring_find_after  (mstring old, mchar c)
{
  return mstring_get_sub_py (old, mstring_find (old, c), -1);
}

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

mstring_a *
mstring_a_split (mstring_a in, mstring c, int len)
{
  mstring_a *  buff       =  malloc(sizeof(mstring_a*));
  int          curr       =  0;
  int          index      =  1;
  int          str_index  =  0;
  mstring_a    str_buff   =  mstring_a_new ();
  mstring_a    end        =  mstring_a_new ();
  
  for (curr = 0; curr != len; curr++, str_index++)
  {
    mstring curr_c = in[curr];
    
    if (curr_c == c)
    {
      str_buff[str_index] = curr_c;
      buff[index] = str_buff;
      str_index   =  0;
      str_buff    =  mstring_a_new ();
      index++;
      continue;
    }
    
    str_buff[str_index] = curr_c;
  }
  
  end[0]      = itoa (curr, 10);
  buff[0]     = end;
  
  return buff;
};

int mstring_split_len (mstring in, mchar splt)
{
  int        occ_num  =  0;
  int        curr;
  
  for (;in[curr]; curr++)
  {
    if (in[curr] == splt)
    {
      occ_num++;
    }
  }
  
  return occ_num;
}

mstring_a mstring_split (mstring in, mchar splt)
{
  int        occ_num  =  0;
  int        occs [32];
  int        curr;
  
  for (;in[curr]; curr++)
  {
    if (in[curr] == splt)
    {
      occs[occ_num] = curr;
      occ_num++;
    }
  }
  occs [occ_num+1] = -1;
  
  mstring_a      buff = mstring_a_new ();
  
  if (occ_num == 0)
  {
    buff[0] = in;
  }
  
  for (curr=0; curr != occ_num; curr++)
  {
    if (curr == 0)
    {
      buff[curr] = mstring_get_sub_py (in, 0, occs[curr]);
    }
    else
    {
      buff[curr] = mstring_get_sub_py (in, occs[curr-1] + 1, occs[curr]);
    }
  }
  buff[curr] = mstring_get_sub_py (in, occs[curr-1] + 1, -1);
  
  return buff;
}

int
mstring_find_start_num (mstring in, mchar find, int start, int num)
{
  int curr  = start;
  int found = 0;
  
  for (;curr != mstring_get_length (in); curr++)
  {
    mchar c = in[curr];
    if (c == find)
    {
      found++;
      if (found == num)
      {
        return curr;
      }
    }
  }
  
  return curr;
}

int
mstring_a_find (mstring_a in, mstring search)
{
  int curr;
  
  for (curr = 0; in [curr]; curr++)
  {
    if (in[curr] == search)
    {
      return curr;
    }
  }
  
  return -1;
}

mstring concat(mstring s1, mstring s2)
{
  mstring   result  =  malloc   (strlen(s1)+strlen(s2)+1);
  
  strcpy                        (result, s1);
  strcat                        (result, s2);
  return result;
}

int mstring_split_quote_len (mstring in, mchar c)
{
  int      occ_num = 0;
  int      curr = 0;
  bool     in_quote = FALSE;
  
  for (; in[curr]; curr++)
  {
    if (in[curr] == '"')
    {
      in_quote = !in_quote;
    }
    
    if (in_quote)
    {
      continue;
    }
    
    if (in[curr] == c)
    {
      occ_num++;
    }
  }
  
  occ_num++;
  
  return occ_num;
}

mstring_a
mstring_split_quote (mstring in, mchar c)
{
  int      occ_num = 0;
  int     *occs = malloc(sizeof(int*));
  int      curr = 0;
  bool     in_quote = FALSE;
  
  for (; in[curr]; curr++)
  {
    if (in[curr] == '"')
    {
      in_quote = !in_quote;
    }
    
    if (in_quote)
    {
      continue;
    }
    
    if (in[curr] == c)
    {
      occs[occ_num] = curr;
      occ_num++;
    }
  }
  
  mstring_a      buff = mstring_a_new ();
  
  if (occ_num == 0)
  {
    buff[0] = in;
  }
  
  for (curr=0; curr != occ_num; curr++)
  {
    if (curr == 0)
    {
      buff[curr] = mstring_get_sub_py (in, 0, occs[curr]);
    }
    else
    {
      buff[curr] = mstring_get_sub_py (in, occs[curr-1] + 1, occs[curr]);
    }
  }
  buff[curr] = mstring_get_sub_py (in, occs[curr-1] + 1, -1);
  
  free(occs);
  
  return buff;
}

int mstring_search(mstring src, mstring str) {
   int i, j, firstOcc;
   i = 0, j = 0;
 
   while (src[i]) {
 
      while (src[i] != str[0] && src[i])
         i++;
 
      if (!src[i])
         return -1;
 
      firstOcc = i;
 
      while (src[i] == str[j] && src[i] && str[j]) {
         i++;
         j++;
      }
 
      if (!str[j])
         return (firstOcc);
      if (!src[i])
         return -1;
 
      i = firstOcc + 1;
      j = 0;
   }
   
   return -1;
}

mstring_a
mstring_a_new (void)
{
  return malloc(sizeof (mstring_a));
}

mstring
mstring_grate (mstring in)
{
  return mstring_get_sub_py (in, 1, mstring_get_length (in) - 1);
}
