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


#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include "../config/parse_config.hpp"
#include "package.hpp"

class ebuild
{
	public:
	string REQUIRED_USE; //!< A list of assertions that must be met by the configuration of Use flags to be valid for this ebuild. (Requires EAPI>=4.)
	vector < string > CURRENT_USE; //!< Found by executing the 'equery uses' command to get the currently enabled strings, this vectors only holds the name of the Use flag
	
	ebuild ( Package package )
	{
		string sourceCmd ( "source " + package.file + " 2> /dev/null && echo $REQUIRED_USE" );
		REQUIRED_USE = get_command_str ( sourceCmd.c_str ( ) );
		REQUIRED_USE = REQUIRED_USE.erase ( REQUIRED_USE.length ( ) - 1, 1 );
		string equeryCmd ( "equery uses " + package.name + " > use" );
		system ( equeryCmd.c_str ( ) );
		string writeFlags ( "python3 ../tools/writeFlags.py use \"" + REQUIRED_USE + "\"" );
		system ( writeFlags.c_str ( ) );
	}
};
