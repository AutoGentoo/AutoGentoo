/*
 * encrypt.h
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


#ifndef __AUTOGENTOO_PASSWORD_ENCRYPT__
#define __AUTOGENTOO_PASSWORD_ENCRYPT__

#include <stdio.h>
#include <openssl/md5.h>
#include <command_tools.h>

typedef struct
{
  char*       md5;
  char*       salt;
} password;

password *     password_new                (void);

char*          agmd5_generate              (void);

password *     password_new_md5            (char* string);

int            password_verify             (password*, char*);
#endif
