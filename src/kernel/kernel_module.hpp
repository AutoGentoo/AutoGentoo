/*
 * kernel_module.hpp
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


#ifndef __AUTOGENTOO_KERNEL_MODULE__
#define __AUTOGENTOO_KERNEL_MODULE__

#include <iostream>
#include <string>
#include "../portage/tools/_misc_tools.hpp"

using namespace std;

string get_type(string input)
{
	if (input == "y" || input == "n" || input == "m")
	{
		return "bool";
	}
	vector<char> nums;
	nums.push_back('0');
	nums.push_back('1');
	nums.push_back('2');
	nums.push_back('3');
	nums.push_back('4');
	nums.push_back('5');
	nums.push_back('6');
	nums.push_back('7');
	nums.push_back('8');
	nums.push_back('9');
	string buff;
	for (size_t i = 0; i != input.length(); i++)
	{
		char c = input[i];
		if (c == '\"')
		{
			return "string";
		}
		if (c == '-')
		{
			return "int";
		}
		if (misc::find<char>(nums, c) != -1)
		{
			buff = "int";
		}
		else
		{
			return "";
		}
	}
	return buff;
}

class KERNEL_MODULE
{
	public:
	string name;
	string value;
	string type;
	bool value_bool;
	bool valid;
	
	KERNEL_MODULE ( string input )
	{
		if ( input [ 0 ] == '#' and input.substr ( 0, 8 ) != "# CONFIG" )
		{
			valid = false;
			return;
		}
		init ( input );
		valid = true;
	}
	void init ( string input )
	{
		size_t sp ( input.find ( "=" ) );
		if ( sp == string::npos )
		{
			value = "n";
			value_bool = false;
			name = misc::remove_r ( misc::remove_r ( input, "# " ), " is not set" );
			return;
		}
		value = misc::substr ( input, sp + 1, input.length ( ) );
		name = misc::substr ( input, 0, sp );
		if ( value == "y" or value == "m" )
		{
			value_bool = true;
		}
		else if ( value == "n" )
		{
			value_bool = false;
		}
		type = get_type(value);
	}
	
	string set_value ( string val )
	{
		return name + "=" + val;
	}
};
#endif
