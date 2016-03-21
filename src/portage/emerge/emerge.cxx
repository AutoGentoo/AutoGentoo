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
	vector<string> argv;
	string cfg;
	string pkgcfg;
	string buff;
	string order;
	string options;
	string misc ("False");
	bool pretend = true;
	bool no_real = true;
	for ( int i = 1; i != argc; i++ )
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
	if ( argc > 4 )
	{
		buff = argv [ 3 ];
		if ( buff == "false" or buff == "False" )
		{
			pretend = false;
		}
		else
		{
			pretend = true;
		}
	}
	if ( argc > 5 )
	{
		buff = argv [ 4 ];
		if ( buff == "false" or buff == "False" )
		{
			no_real = false;
		}
		else
		{
			no_real = true;
		}
	}
	if ( argc > 6 )
	{
		order = argv [ 5 ];
	}
	if ( argc > 7 )
	{
		options = argv [ 6 ];
	}
	if ( argc > 8 )
	{
		misc = argv [ 7 ];
	}
	Emerge ( argv [ 0 ], cfg, pkgcfg, pretend, options );
	if ( !no_real )
	{
		string cmd ( "python3 ../package/package.py " + pkgcfg + " ../package/logs " + order + " False" + misc );
		system ( cmd.c_str ( ) );
	}
	
	return 0;
}
