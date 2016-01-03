/*
 * parse_config.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atadmin@Helios>
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
#include <vector>
#include <map>

template < class Key, class T=void >
class getVariables
{
	public:
	
	std::map < Key, T > mapped;
	std::string str;
	
	getVariables ( std::string input )
	{
		std::vector < std::string > variables;
		std::vector < std::string > values;
		std::string buff_str;
		std::vector < std::string > vec;
		bool is_var = true;
		
		for ( unsigned int y; y <= input.length ( ); y++ )
		{
			if ( is_var )
			{
				vec = &variables;
			}
			else
			{
				vec = &values;
			}
			char x = input.at ( y );
			if ( x == '=' )
			{
				vec.push_back ( buff_str ); 
			}
			buff_str.append ( std::string ( x ) );
			if ( x == '=' )
			{
				buff_str.clear ( );
				is_var = false;
			}
			if ( x == ' ' )
			{
				buff_str.clear ( );
				is_var = true;
			}
		}
		
		//if ( typeid ( T ) == typeid ( v
	}
};
