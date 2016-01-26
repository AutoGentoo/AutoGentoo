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

#ifndef __PARSE_CONFIG__
#define __PARSE_CONFIG__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include "../tools/_misc_tools.hpp"
#include "../tools/file.hpp"

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
		misc::remove ( var, " " );
		misc::remove ( val, "\"" );
		misc::remove ( val, "=" );
		misc::remove ( val, "\"" );
		std::vector<std::string> _val = misc::split ( val, ' ' );
		buff_map[var] = _val;
	}
	return buff_map;
}

std::string get_env ( std::string var )
{
	ofstream file ("temp.sh");
	std::string fileStr ( "#!/bin/bash\nvar=\necho $var" );
	file << fileStr;
	std::string cmd ( "sh temp.sh --var=$" + var + " > temp" );
	system ( cmd.c_str ( ) );
	std::string returnStr ( File ( "temp.sh" ).read ( ) );
	system ( "rm -rf temp" );
	system ( "rm -rf temp.sh" );
	return returnStr;
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
		misc::remove ( var, " " );
		misc::remove ( val, "\"" );
		misc::remove ( val, "=" );
		misc::remove ( val, "\"" );
		buff_map[var] = val;
	}
	return buff_map;
}
std::string get_command_str ( std::string cmd )
{
	std::string return_str;
	cmd += " > temp";
	system ( cmd.c_str ( ) );
	File f ( "temp" );
	return_str = f.read ();
	return return_str;
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
		std::vector<std::string> divide = misc::split ( line, ':' );
		std::string var, val;
		var = divide[0];
		val = divide[1];
		trim ( val );
		return_map[var] = val;
	}
	system("rm -rf temp");
	return return_map;
}

std::vector < std::string > get_command_vec_raw ( std::string cmd )
{
	std::vector < std::string > buff;
	cmd += " > temp";
	system ( cmd.c_str ( ) );
	buff = File ( "temp" ).readlines ( );
	system("rm -rf temp");
	return buff;
}
#endif
