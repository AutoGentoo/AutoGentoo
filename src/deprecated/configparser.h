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
#include "file.h"

typedef struct
{
  mstring         name;
  mstring         value;
  mstring_a       arguments;
  mstring         full_value;
} Variable; 

typedef struct
{
  mstring         name;
  Variable       *variables;
  int             varc;
  mstring_a       comments;
} Section;

typedef struct
{
  mstring         file;
  mstring_a       comments;
  
  Section        *sections;
  int             sectionc;
} Config;

Variable          variable_new                    (void);

Variable          variable_new_from_str           (mstring);

Section           section_new                     (void);

Section           section_new_from_str            (mstring_a);

Config            config_new                      (void);

Config            config_new_from_str             (mstring);

void              variable_free                   (Variable*);

void              section_free                    (Section*);

void              config_free                     (Config*);

mstring           section_get_value               (Section, mstring);

mstring           config_get_value                (Config, mstring, mstring);

#endif
