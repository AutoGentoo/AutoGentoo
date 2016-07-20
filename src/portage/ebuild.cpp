/*
 * ebuild.hpp
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

#include "ebuild.h"

ebuild::ebuild ( Package package )
{
  string sourceCmd ( "source " + package.file + " 2> /dev/null && echo $REQUIRED_USE" );
  REQUIRED_USE = get_command_str ( sourceCmd );
  REQUIRED_USE = REQUIRED_USE.erase ( REQUIRED_USE.length ( ) - 1, 1 );
  string equeryCmd ( "equery uses " + package.name + " > use" );
  system ( equeryCmd.c_str ( ) );
  string writeFlags ( "python3 writeFlags.py use \"" + REQUIRED_USE + "\"" );
  system ( writeFlags.c_str ( ) );
  SUGGEST_USE = get_command_str ( "python3 parseFlags.py" );
  useGentooFlag _use ( package.path, SUGGEST_USE, true );
}
