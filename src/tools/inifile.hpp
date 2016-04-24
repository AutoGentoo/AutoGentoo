/*
 * inifile.cxx
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

#ifndef __AUTOGENTOO_INIFILE__
#define __AUTOGENTOO_INIFILE__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "file.hpp"

class __INIFILE__
{
	public:
	vector < string > sections;
	vector < string > loaded;
	map < string, vector < int > > section_to_line;
	string file_name;
	vector < string > file;
	
	void load ( string __file_name )
	{
		file_name = __file_name;
		file = File ( file_name ).readlines ( );
		vector < int > temp_lines;
		string curr_sec;
		for ( size_t i = 0; i != file.size ( ); i++ )
		{
			string curr_line = file [ i ];
			if ( curr_line [ 0 ] == '[' )
			{
				temp_lines.clear ( );
				curr_sec = curr_line.erase ( 0, 1 ).erase ( curr_line.length ( ), 1 );
				temp_lines.push_back ( i );
			}
			if ( curr_line [ 0 ] == ']' )
			{
				temp_lines.push_back ( i );
				sections.push_back ( curr_sec );
				section_to_line [ curr_sec ] = temp_lines;
			}
			if ( curr_line.substr ( 0, 6 ) == "source" )
			{
				string temp_filename ( misc::substr ( curr_line, 7, curr_line.length ( ) ) );
				__INIFILE__ TEMP;
				TEMP.load ( temp_filename );
				loaded.push_back ( temp_filename );
			}
		}
	}
	
	int start ( string sec )
	{
		return section_to_line [ sec ] [ 0 ];
	}
	
	int end ( string sec )
	{
		return section_to_line [ sec ] [ 1 ];
	}
};
#endif
