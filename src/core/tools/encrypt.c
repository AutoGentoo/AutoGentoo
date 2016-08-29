/*
 * encrypt.c
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


#include <encrypt.h>

password *
password_new (void)
{
  password     *buff   =  palloc(sizeof(password*));
  buff->md5            =  "";
  buff->salt           =  "";
  return buff;
}

char*
agmd5_generate (void)
{
  char* buff = palloc (sizeof (char) * 32);
  buff = mstring_find_before(get_output ("date | md5sum"), ' ');
  return buff;
}


password *
password_new_md5 (char* string)
{
  password * pbuff = password_new ();
  pbuff->salt = agmd5_generate ();
  
  char* cmdbuff = palloc (sizeof(char) * 1082);
  sprintf(cmdbuff, "openssl passwd -1 -salt %s %s", pbuff->salt, string);
  pbuff->md5 = mstring_find_before(get_output (cmdbuff), '\n');
  return pbuff;
}

int
password_verify (password* inpwd, char* checkpwd)
{
  char* cmdbuff = palloc (sizeof(char) * 1082);
  sprintf(cmdbuff, "openssl passwd -1 -salt %s %s", inpwd->salt, checkpwd);
  int out = strcmp(mstring_find_before(get_output (cmdbuff), '\n'), inpwd->md5) == 0;
  return out;
}
