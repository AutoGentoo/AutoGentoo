/*
 * kernel.hpp
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

#ifndef __AUTOGENTOO_KERNEL_CONFIG__
#define __AUTOGENTOO_KERNEL_CONFIG__

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "kernel_module.hpp"
#include "../portage/tools/_misc_tools.hpp"
#include "../portage/config/parse_config.hpp"

using namespace std;

class kernel
{
	public:
	vector < KERNEL_MODULE > MODULES;
	vector < int > LINE_NUMBERS;
	vector < string > MODULES_NAMES;
	vector < string > __file;
	string cfg;
	
	kernel ( string _cfg = "/usr/src/linux/.config" )
	{
		cfg = _cfg;
		load ( cfg );
	}
	
	void load ( string __cfg )
	{
		__file = File ( __cfg ).readlines ( );
		for ( size_t i = 0; i != __file.size ( ); i++ )
		{
			KERNEL_MODULE curr ( __file [ i ] );
			if ( curr.valid )
			{
				LINE_NUMBERS.push_back ( i );
				MODULES_NAMES.push_back ( curr.name );
				MODULES.push_back ( curr );
			}
		}
	}
	
	void write ( string of = "/usr/src/linux/.config" )
	{
		ofstream output;
		string trunc ( "truncate -s 0 " + of );
		system ( trunc.c_str ( ) );
		output.open ( of.c_str ( ), ofstream::trunc );
		for ( size_t i = 0; i != __file.size ( ); i++ )
		{
			if ( i == 119 )
			{
				cout << __file [ i ] << endl;
			}
			output << __file [ i ] << endl;
		}
		output.close ( );
	}
	
	vector < string > find_modules ( string SEARCH )
	{
		replace( SEARCH.begin(), SEARCH.end(), '-', '_');
		vector < string > buff;
		buff = get_command_vec_raw ( "cat " + cfg + " | grep -i \"" + SEARCH + "\"" );
		return buff;
	}
	
	void set_module ( string NAME, string VAL )
	{
		if ( NAME.substr ( 0, 7 ) != "CONFIG_" )
		{
			NAME = "CONFIG_" + NAME;
		}
		int found = misc::find_spec ( MODULES_NAMES, NAME, 0, NAME.length ( ) );
		if ( found != -1 )
		{
			cout << MODULES [ found ].name << endl;
			cout << LINE_NUMBERS [ found ] << endl;
			__file = misc::set_vec < string > ( __file, LINE_NUMBERS [ found ], MODULES [ found ].set_value ( VAL ) );
		}
	}
};
#endif
