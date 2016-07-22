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
#include "mlib.h"

#ifndef AUTOGENTOO_MSTRING
#define AUTOGENTOO_MSTRING

/** mstring stands for Minux String
 * This type is the main string type for this software
**/

mstring            mstring_new                 (void);

mstring            mstring_new_from_char       (mchar);

mstring            mstring_new_from_chars      (char *);

mstring            mstring_get_sub             (mstring, int, int);

mstring            mstring_get_sub_py          (mstring, int, int);

mstring            mstring_find_before         (mstring, mchar);

mstring            mstring_find_after          (mstring, mchar);

int                mstring_get_length          (mstring);

int                mstring_find_start          (mstring, mchar, int);

int                mstring_find_start_num      (mstring, mchar, int, int);

int                mstring_find                (mstring, mchar);

int                mstring_rfind               (mstring, mchar);

mstring_a          mstring_a_new               (void);

int                mstring_split_len           (mstring, mchar);

int                mstring_split_quote_len     (mstring, mchar);

mstring_a          mstring_split               (mstring, mchar);

mstring_a *        mstring_a_split             (mstring_a, mstring, int);

int                mstring_a_find              (mstring_a, mstring);

mstring            concat                      (mstring, mstring);

mstring_a          mstring_split_quote         (mstring, mchar);

int                mstring_search              (mstring, mstring);

mstring            itoa                        (int, int);

mstring            mstring_grate               (mstring);
#endif
