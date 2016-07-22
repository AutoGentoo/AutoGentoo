/*
 * configparser.h
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


#ifndef __AUTOGENTOO_CONFIG_PARSER__
#define __AUTOGENTOO_CONFIG_PARSER__

#include <stdio.h>
#include "mstring.h"

typedef struct
{
  mstring         name;
  mstring         value;
  mstring_a       arguments;
  mstring         full_value;
  int             argc;
  int             type; /** 0: variable, 1: argument, 2: list item **/
} Variable; 

typedef struct
{
  mstring         name;
  int            *variabletypes; /** 0: variable, 1: argument, 2: list item **/
  Variable       *variables;
  int             varc;
  
} Section;

typedef struct
{
  mstring         file;
  
  Section        *sections;
  int             sectionc;
} Config;

Variable *        variable_new                    (void);

Variable *        variable_new_from_str           (mstring);

Section *         section_new                     (void);

Section *         section_new_from_str            (mstring);

Config *          config_new                      (void);


#endif
