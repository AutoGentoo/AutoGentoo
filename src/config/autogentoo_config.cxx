/*
 * autogentoo_config.cxx
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

#ifndef __AUTOGENTOO_CONFIG_READING__
#define __AUTOGENTOO_CONFIG_READING__

#include <iostream>
#include <map>
#include "../portage/tools/_misc_tools.hpp"
#include "../portage/tools/file.hpp"

using namespace std;

class AutoGentoo_Config
{
	public:
	string file_name;
	vector < string > sections;
	vector < string > lines;
	map < string, map < string, string > > variables;
	
	void load ( string _file_name )
	{
		file_name = _file_name;
		lines = File ( file_name ).readlines ( );
		string curr_section;
		for ( size_t i = 0; i != lines.size ( ); i++ )
		{
			map < string, string > buff_map;
			string curr ( lines [ i ] );
			if ( curr [ 0 ] == '#' )
			{
				continue;
			}
			if ( curr [ 0 ] == '[' )
			{
				curr_section = misc::remove_r ( misc::remove_r ( curr, "[" ), "]" );
				sections.push_back ( curr_section );
				continue;
			}
			vector < string > val_split ( misc::split ( curr, '=', true ) );
			buff_map [ val_split [ 0 ] ] = val_split [ 1 ];
			variables [ curr_section ] = buff_map;
		}
	}
	
	map < string, string > operator [ ] ( string sect )
	{
		return variables [ sect ];
	}
};
#endif
