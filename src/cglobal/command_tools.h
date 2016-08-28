/*
 * command_tools.h
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

#ifndef __AUTOGENTOO_COMMAND_TOOL_H__
#define __AUTOGENTOO_COMMAND_TOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <cglobal/mstring.h>
#include <cglobal/file.h>

mstring         get_output          (mstring);

mstring_a       get_output_lines    (mstring);

int             get_output_length   (mstring __command);

#endif
