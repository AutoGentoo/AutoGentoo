/*
 * drivers.cxx
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
#include <stdlib.h>
#include "../command/Option.hpp"

using namespace std;

int main ( int argc, char *argv [ ] )
{
	string input;
	string nvidia_install;
	string nvidia_download;
	
	for ( int i = 0; i != argc; i++ )
	{
		input += argv [ i ];
		input += " ";
	}
	
	OptionSet driver_opts;
	driver_opts.init ( "drivers", "Find and install the supported drivers for your nvidia graphics card" );
	
	driver_opts.add_option ( "install", "true", "i", "bool", "Specify whether to install the drivers" );
	driver_opts.add_option ( "download", "true", "d", "bool", "Specify whether to download the drivers" );
	driver_opts.add_option ( "create-xconfig", "true", "X", "bool", "Create the x-config file for updated drivers" );
	
	driver_opts.create_help ( );
	driver_opts.feed ( input );
	
	
	if ( driver_opts [ "download" ] )
	{
		nvidia_download = "wget $ ( python get_dist.py ) && chmod +x NVIDIA*";
	}
	else
	{
		nvidia_download = "";
	}
	
	if ( driver_opts [ "create-xconfig" ] )
	{
		nvidia_install = "./NVIDIA* -X -a";
	}
	else
	
	{
		nvidia_install = "./NVIDIA* -a";
	}
	
	if ( !driver_opts [ "install" ] )
	{
		nvidia_install = "";
	}
	
	system ( nvidia_download.c_str ( ) );
	system ( nvidia_install.c_str ( ) );
	return 0;
}

