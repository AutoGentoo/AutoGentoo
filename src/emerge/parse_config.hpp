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
#include <boost/algorithm/string.hpp>
#include "formatString.hpp"
#include "file.hpp"

using namespace std;
using namespace boost::algorithm;

std::map < std::string, std::vector< std::string > > get_variables_split ( std::string input )
{
	std::map < std::string, std::vector<std::string> > buff_map;
	std::vector < std::string > variables, values, *vec;
	std::string buff_str;
	bool is_var = true;
	
	for ( size_t y = 0; y != input.length ( ); y++ )
	{
		if ( is_var )
		{
			vec = &variables;
		}
		else
		{
			vec = &values;
		}
		
		char curr = input[y];
		if ( is_var )
		{
			if ( curr == '=' )
			{
				is_var = false;
				vec->push_back ( buff_str );
				buff_str.clear ( );
			}
			else
			{
				buff_str += curr;
			}
		}
		if ( !is_var )
		{
			if ( curr == '\"' && ( input[y+1] == ' ' || y+1 == input.length ( ) ) )
			{
				is_var = true;
				vec->push_back ( buff_str );
				buff_str.clear ( );
			}
			else
			{
				buff_str += curr;
			}
		}
	}
	for ( size_t i = 0; i != variables.size ( ); i++ )
	{
		std::string var = variables[i];
		std::string val = values[i];
		strfmt::remove ( var, " " );
		strfmt::remove ( val, "\"" );
		strfmt::remove ( val, "=" );
		strfmt::remove ( val, "\"" );
		std::vector<std::string> _val = strfmt::split ( val, ' ' );
		buff_map[var] = _val;
	}
	return buff_map;
}

std::map < std::string, std::string > get_variables ( std::string input )
{
	std::map < std::string, std::string > buff_map;
	std::vector < std::string > variables, values, *vec;
	std::string buff_str;
	bool is_var = true;
	
	for ( size_t y = 0; y != input.length ( ); y++ )
	{
		if ( is_var )
		{
			vec = &variables;
		}
		else
		{
			vec = &values;
		}
		
		char curr = input[y];
		if ( is_var )
		{
			if ( curr == '=' )
			{
				is_var = false;
				vec->push_back ( buff_str );
				buff_str.clear ( );
			}
			else
			{
				buff_str += curr;
			}
		}
		if ( !is_var )
		{
			if ( curr == '\"' && ( input[y+1] == ' ' || y+1 == input.length ( ) ) )
			{
				is_var = true;
				vec->push_back ( buff_str );
				buff_str.clear ( );
			}
			else
			{
				buff_str += curr;
			}
		}
	}
	for ( size_t i = 0; i != variables.size ( ); i++ )
	{
		std::string var = variables[i];
		std::string val = values[i];
		strfmt::remove ( var, " " );
		strfmt::remove ( val, "\"" );
		strfmt::remove ( val, "=" );
		strfmt::remove ( val, "\"" );
		buff_map[var] = val;
	}
	return buff_map;
}

std::map < std::string, std::string > get_command ( std::string cmd )
{
	std::map < std::string, std::string > return_map;
	cmd += " > temp";
	system ( cmd.c_str() );
	File f ( "temp" );
	std::cout << "file opened" << std::endl;
	std::vector<std::string> lines = f.readlines ( );
	std::cout << "read lines" << std::endl;
	for ( size_t x = 0; x != lines.size ( ); x++ )
	{
		std::string line = lines[x];
		std::cout << line << std::endl;
		std::vector<std::string> divide = strfmt::split ( line, ':' );
		std::cout << "divided" << std::endl;
		std::string var, val;
		std::cout << divide.size ( ) << std::endl;
		var = divide[0];
		std::cout << "shit" << std::endl;
		val = divide[1];
		std::cout << "shit" << std::endl;
		trim ( val );
		std::cout << "shit" << std::endl;
		return_map[var] = val;
		std::cout << "shit" << std::endl;
	}
	system("rm -rf temp");
	return return_map;
}
