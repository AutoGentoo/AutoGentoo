/*
 * emerge.cxx
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
#include "emerge.hpp"
#include "../../command/Option.hpp"

using namespace std;

int main(int argc, char* args[])
{
	string input;
	
	for ( int i = 0; i != argc; i++ )
	{
		input += args [ i ];
		input += " ";
	}
	
	
	OptionSet test;
	test.init ( "emerge", "Use the AutoGentoo portage API to install specified packages" );
	
	test.add_arg ( "PACKAGE" );
	test.add_arg ( "OPTIONS" );
	
	test.add_option ( "emerge", "emerge.pretend", "e", "string", "Specify where the output of emerge --pretend is kept" );
	test.add_option ( "config", "emerge.cfg", "c", "string", "Specify where the output of package config is kept" );
	test.add_option ( "pretend", "true", "p", "bool", "Specify whether to execute emerge --pretend" );
	test.add_option ( "install", "true", "i", "bool", "Specify whether to actually install or whether to just configure" );
	test.add_option ( "order", "None", "r", "string", "Change the stage order of installation" );
	test.add_option ( "options", "", "o", "string", "Select options run with emerge" );
	test.add_option ( "updates", "false", "u", "bool", "Install updates only" );
	test.add_option ( "ebuild-opts", "--color=y", "O", "string", "Select options run with ebuild" );
	test.add_option ( "show-opts", "false", "s", "bool", "Show the current environment variables and exit" );
	
	test.create_help ( );
	test.feed ( input );
	
	if ( test [ "show-opts" ] )
	{
		for ( map < int, option >::iterator i = test.int_to_main.begin ( ); i != test.int_to_main.end ( ); i++ )
		{
			cout << i->second._long << ": " << i->second.value << endl;
		}
		misc::print_vec < string > ( test.cmd_args );
		exit (0);
	}
	
	Emerge ( test.cmd_args [ 0 ], test ( "emerge" ), test ( "config" ), test [ "pretend" ], test ( "options" ), test [ "updates" ] );
	if ( test [ "install" ] )
	{
		string cmd ( "python3 ../package/package.py " + test ( "config" ) + " ../package/logs " + test ( "order" ) + " false false " + test ( "ebuild-opts" ) );
		system ( cmd.c_str ( ) );
	}
	return 0;
}
