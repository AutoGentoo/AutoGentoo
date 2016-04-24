/*
 * option_test.cxx
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
#include "Option.hpp"

using namespace std;

int main(int argc, const char* args [] )
{
	string input;
	
	for ( int i = 0; i != argc; i++ )
	{
		input += args [ i ];
		input += " ";
	}
	
	OptionSet test;
	test.init ( "test", "test program for option.hpp" );
	
	test.add_arg ( "OPTIONS" );
	test.add_arg ( "FILE" );
	
	test.add_option ( "usefile", "false", "u", "bool", "Use the file" );
	test.add_option ( "file", "", "f", "string", "The file" );
	test.add_option ( "print", "", "p", "string", "Print the var" );
	
	test.create_help ( );
	test.feed ( input );
	
	if ( test [ "usefile" ] )
	{
		cout << "true" << endl;
	}
	else
	{
		cout << "false" << endl;
	}
	
	/*
	for ( map < int, option >::iterator i = test.int_to_main.begin ( ); i != test.int_to_main.end ( ); i++ )
	{
		cout << i->second._long << ": " << i->second.value << endl;
	}*/
	return 0;
}

