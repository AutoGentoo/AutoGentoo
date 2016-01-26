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

using namespace std;

int main(int argc, char* args[])
{
	/*vector<string> argv;
	for ( int i = 1; i < argc; i++ )
	{
		argv.push_back ( string ( args[i] ) );
	}
	vector <string > optionsArgs;
	optionsArgs.push_back ( "package" );
	OptionParser options ( "emerge", "A C++ emerge tool by AutoGentoo", optionsArgs, argv );
	
	vector < string > commands;
	vector < vector < string > > cmdArgs;
	commands.push_back ( "fetch" );
	commands.push_back ( "setup" );
	commands.push_back ( "unpack" );
	commands.push_back ( "prepare" );
	commands.push_back ( "configure" );
	commands.push_back ( "compile" );
	commands.push_back ( "test" );
	commands.push_back ( "install" );
	commands.push_back ( "postinstall" );
	commands.push_back ( "prerm" );
	commands.push_back ( "postrm" );
	cmdArgs.push_back ( commands );
	
	Option cmd ( "-c", "--command", "Execute a function in the ebuild", cmdArgs );
	
	options.addOption ( cmd );
	options.createHelp ( );
	if ( options.findOption ( "--help" ) )
	{
		options.showHelp ( );
	}
	*/
	
	vector<string> argv;
	string cfg;
	string pkgcfg;
	for ( int i = 1; i < argc; i++ )
	{
		argv.push_back ( string ( args[i] ) );
	}
	if ( argc > 2 )
	{
		cfg = argv [ 1 ];
	}
	if ( argc > 3 )
	{
		pkgcfg = argv [ 2 ];
	}
	Emerge ( argv [ 0 ], cfg, pkgcfg );
	return 0;
}
