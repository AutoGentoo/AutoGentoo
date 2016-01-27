/*
 * kernel_test.cxx
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
#include "kernel.hpp"
#include <string>
#include "../command/command_line.hpp"

using namespace std;

kernel __kernel;

void __kernel_set_module ( )
{
	string _NAME;
	string _VAL;
	cout << "name> ";
	cin >> _NAME;
	cout << "value> ";
	cin >> _VAL;
	__kernel.set_module ( _NAME, _VAL );
}
void __kernel_get_module ( )
{
	string _NAME;
	cout << "name> ";
	cin >> _NAME;
	misc::print_vec < string > ( __kernel.find_modules ( _NAME ) );
}

void __kernel_write ( )
{
	string FILE;
	cout << "file> ";
	cin >> FILE;
	__kernel.write ( FILE );
}

int main(int argc, char **argv)
{
	__kernel.load ( "/usr/src/linux/.config" );
	CommandLine COMMAND ( "kernel> " );
	string cmd;
	COMMAND.addOption ( "set_module", __kernel_set_module );
	COMMAND.addOption ( "get_module", __kernel_get_module );
	COMMAND.addOption ( "write", __kernel_write );
	COMMAND.start ( cmd );
	
	return 0;
}

