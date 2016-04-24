/*
 * set.hpp
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

#ifndef __AUTOGENTOO_SET__
#define __AUTOGENTOO_SET__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../tools/file.hpp"
#include "../tools/_misc_tools.hpp"
#include "../tools/inifile.hpp"
#include "config_pkg.hpp"

using namespace std;

/*
 * 1:description
 * 2:profile
 * 3:dependency
 * 4:option
 * 5:file_exec
 * 6:package
*/

int line_type ( string LINE )
{
	if ( LINE [ 0 ] == '!' )
	{
		return 1;
	}
	
	else if ( LINE [ 0 ] == ':' )
	{
		return 2;
	}
	
	else if ( LINE [ 0 ] == '?' )
	{
		return 3;
	}
	
	else if ( LINE [ 0 ] == '|' )
	{
		return 4;
	}
	
	else if ( LINE.substr ( 0, 4 ) == "exec" )
	{
		return 5;
	}
	
	else if ( !LINE.empty ( ) )
	{
		return 6;
	}
}

class SET
{
	public:
	vector < CONFIG_PKG > pkgs;
	vector < SET > deps;
	string section_name;
	
	vector < string > content;
	
	vector < string > options;
	string desc;
	string profile;
	
	void init ( __INIFILE__ &top, string section )
	{
		section_name = section;
		for ( int i = top.start ( section_name ); i != top.end ( section_name ); i++ )
		{
			content.push_back ( top.file [ i ] );
		}
		
		for ( size_t i = 0; i != content.size ( ); i++ )
		{
			int type ( line_type ( content [ i ] ) );
			if ( type == 1 )
			{
				desc = misc::substr ( content [ i ], 1 ,content [ i ].length ( ) );
			}
			if ( type == 2 )
			{
				profile = misc::substr ( content [ i ], 1, content [ i ].length ( ) );
			}
			
			if ( type == 3 )
			{
				vector < string > deps_name = misc::split ( misc::substr ( content [ i ], 1, content [ i ].length ( ) ), ' ', true );
				for ( size_t i = 0; i != deps_name.size ( ); i++ )
				{
					SET TEMP;
					TEMP.load ( top, deps_name [ i ] );
					deps.push_back ( TEMP );
				}
			}
			
			if ( type == 4 )
			{
				options = misc::split ( misc::substr ( content [ i ], 1, content [ i ].length ( ) ), ' ', true );
			}
			
			if ( type == 5 )
			{
				profile = misc::substr ( content [ i ], 1, content [ i ].length ( ) );
			}
			
			if ( type == 2 )
			{
				profile = misc::substr ( content [ i ], 1, content [ i ].length ( ) );
			}
			
		}
	}
};

#endif
