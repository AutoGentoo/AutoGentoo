/*
 * mstring.h
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "ptr_handler.h"

#ifndef AUTOGENTOO_MSTRING
#define AUTOGENTOO_MSTRING

char*              mstring_new_from_char       (char );

char*              mstring_new_from_chars      (char *);

char*              mstring_get_sub             (char*, int, int);

char*              mstring_get_sub_py          (char*, int, int);

char*              mstring_find_before         (char*, char );

char*              mstring_find_after          (char*, char );

int                mstring_get_length          (char*);

int                mstring_find_start          (char*, char , int);

int                mstring_find_start_num      (char*, char , int, int);

int                mstring_find                (char*, char );

int                mstring_rfind               (char*, char );

int                mstring_split_len           (char*, char );

int                mstring_split_quote_len     (char*, char );

char**             mstring_split               (char*, char );

char**             mstring_split_str           (char*, char*);

char***            mstring_a_split             (char**, char*, int);

int                mstring_a_find              (char**, char*);

char*              concat                      (char*, char*);

char**             mstring_split_quote         (char*, char );

int                mstring_search              (char*, char*);

char*              itoa                        (int, int);

char*              mstring_grate               (char*);

char*              mstring_removechar          (char*, char );

int                mstring_a_get_length        (char**);

//const char*      rprintf                     (const char* format, ...);

void               aprint                      (char**);

void               print                       (char* format, ...);

int                systemf                     (const char* format, ...);

void               mstring_a_hp_stk            (char**, char**);

#endif
