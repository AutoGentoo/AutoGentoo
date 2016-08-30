/*
 * mstring.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is pfree software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the pfree Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the pfree Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <mstring.h>
int
mstring_get_length (char* str)
{
  int num;
  for (num = 0; str[num]; num++);
  return num;
}

char*
mstring_get_sub  (char* old, int start, int length)
{
  int  size = mstring_get_length (old);
  char* out_array = palloc (sizeof(char) * (length + 1));
  
  if (size < (start+length))
  {
    length = size-start;
  }
  
  int curr;
  
  for (curr = 0; curr != length; curr++)
  {
    out_array [curr] = old[curr+start];
  }
  
  return out_array;
}

char*
mstring_get_sub_py (char* old, int start, int end)
{
  int      size = mstring_get_length (old);
  int      len;
  char    *out_array = malloc (sizeof (char) * (size + 4));
  
  if (size < end)
  {
    end = size;
  }
  
  if (end < 0)
  {
    end = size + end;
    end++;
  }
  
  if (start < 0)
  {
    start = size + start;
    start++;
  }
  
  len = end - start;
  memcpy(out_array, &old[start], len);
  
  if (out_array[len] != '\0')
  {
    out_array[len + 1] = '\0';
  }
  
  return out_array;
}

int
mstring_find (char* in, char find)
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
mstring_rfind (char* in, char find)
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
mstring_find_start (char* in, char find, int start)
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

char*
mstring_find_before  (char* old, char c)
{
  return mstring_get_sub_py (old, 0, mstring_find (old, c));
}

char*
mstring_find_after  (char* old, char c)
{
  return mstring_get_sub_py (old, mstring_find (old, c), -1);
}

char* itoa(int val, int base)
{
  static char buf[32] = {0};
  
  int i = 30;
  
  for(; val && i ; --i, val /= base)
  {
    buf[i] = "0123456789abcdef"[val % base];
  }
  
  return &buf[i+1];
  
}

char***
mstring_a_split (char** in, char* c, int len)
{
  char** *  buff       =  palloc(sizeof(char**) * mstring_a_get_length (in));
  int          curr       =  0;
  int          index      =  1;
  int          str_index  =  0;
  char**    str_buff   =  palloc (sizeof(char*) * mstring_a_get_length (in));
  char**    end        =  palloc (sizeof(char*) * mstring_a_get_length (in));
  
  for (curr = 0; curr != len; curr++, str_index++)
  {
    char* curr_c = in[curr];
    
    if (strcmp (curr_c, c) == 0)
    {
      str_buff[str_index] = curr_c;
      buff[index] = str_buff;
      str_index   =  0;
      str_buff    =  palloc (sizeof(char*) * mstring_a_get_length (in));
      index++;
      continue;
    }
    
    str_buff[str_index] = curr_c;
  }
  
  end[0]      = itoa (curr, 10);
  buff[0]     = end;
  
  pfree(str_buff);
  pfree(end);
  
  return buff;
};

int mstring_split_len (char* in, char splt)
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

char** mstring_split (char* a_str, char a_delim)
{
  int a = 0;
  int c = 0;
  for (a=0; a_str[a]; a++)
  {
    if (a_str[a] == a_delim)
    {
      c++;
    }
  }
  c++;
  char **out = calloc (c+2, sizeof(char*));
  
  char *buffer = palloc (sizeof(char) * (a+1));
  int  c_buff = 0;
  int first = 0;
  int i = 0;
  for (i=0; c_buff != c; i++)
  {
    if (!a_str[i])
    {
      break;
    }
    
    if (a_str[i] == a_delim)
    {
      if (!buffer || strcmp (buffer, "") == 0)
        break;
      out[c_buff] = palloc (sizeof(char) * (mstring_get_length (buffer) + 5));
      strcpy (out[c_buff], buffer);
      memset(buffer, 0, mstring_get_length(buffer));
      first = 0;
      c_buff++;
      continue;
    }
    
    if (first)
    {
      sprintf (buffer, "%s%c", buffer, a_str[i]);
    }
    else
    {
      sprintf (buffer, "%c", a_str[i]);
      first = 1;
    }
  }
  
  out[c_buff] = palloc (sizeof(char) * (mstring_get_length (buffer) + 5));
  strcpy (out[c_buff], buffer);
  c_buff++;
  
  if (buffer)
    pfree (buffer);
  return out;
}

char** mstring_split_str (char* in, char* delim)
{
  char* buff_out = palloc (sizeof(char) * mstring_get_length (in));
  strcpy (buff_out, in);
  char* buff;
  char* token;
  
  int count = 0;
  const char *tmp = in;
  while(tmp == strstr(tmp, delim))
  {
     count++;
     tmp++;
  }
  
  char** out = palloc (sizeof (char*) * count + 1);
  
  int curr = 0;
  
  if (buff_out != NULL)
  {
    buff = in;

    while ((token = strsep(&in, delim)) != NULL)
    {
      out[curr] = token;
      curr++;
    }
  
    pfree(buff);
  }
  
  pfree (buff_out);
  
  return out;
}

int
mstring_a_find (char** in, char* search)
{
  int curr;
  
  for (curr = 0; in [curr]; curr++)
  {
    if (strcmp(in[curr], search) == 0)
    {
      return curr;
    }
  }
  
  return -1;
}

char* concat(char* s1, char* s2)
{
  char*   result  =  palloc   (mstring_get_length(s1)+mstring_get_length(s2)+1);
  
  strcpy                        (result, s1);
  strcat                        (result, s2);
  return result;
}

int mstring_split_quote_len (char* in, char c)
{
  int      occ_num = 0;
  int      curr = 0;
  int      in_quote = 0;
  
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

char**
mstring_split_quote (char* in, char c)
{
  int      occ_num = 0;
  int     *occs = palloc(sizeof(int*));
  int      curr = 0;
  int      in_quote = 0;
  
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
  
  char**      buff = palloc (sizeof(char*) * mstring_get_length (in));
  
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
  
  pfree(occs);
  
  return buff;
}

int mstring_search(char* src, char* str) {
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

char*
mstring_grate (char* in)
{
  return mstring_get_sub_py (in, 1, mstring_get_length (in) - 1);
}

char*
mstring_removechar (char* in, char chr)
{
  char* out = palloc(sizeof(char*) * mstring_get_length (in));
  int curr;
  int out_curr;
  for (curr = 0, out_curr = 0; in[curr]; curr++)
  {
    if (in[curr] != chr)
    {
      out[out_curr] = in[curr];
      out_curr++;
    }
  }
  
  return out;
}

int
mstring_a_get_length (char** in)
{
  int out;
  for (out=0; in[out]; out++);
  return out;
}

const char*
rprintf (const char* format, ...)
{
  char       msg[1024];
  memset(&msg[0], 0, sizeof(msg));
  va_list    args;

  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args);
  va_end(args);
  
  char* out = msg;
  
  return out;
}

void
aprint (char** in)
{
  int i;
  for (i=0; in[i]; i++)
  {
    printf ("%s\n", in[i]);
  }
}

void
print (char* format, ...)
{
  if (!format)
  {
    return;
  }
  
  char       msg[1024];
  va_list    args;

  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args); // do check return value
  va_end(args);

  printf ("%s\n", msg);
  fflush (stdout);
}

int
systemf (const char* format, ...)
{
  char       msg[1024];
  va_list    args;
  
  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args); // do check return value
  va_end(args);

  return system(msg);
}

void mstring_a_hp_stk (char** __dest, char** __src)
{
  int i;
  for (i=0; __src[i]; i++)
  {
    __dest[i] = __src[i];
  }
  
  array_free ((void**)__src);
}
