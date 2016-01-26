/*
 * command_line.hpp
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


#ifndef __AUTOGENTOO_CMD_LINE__
#define __AUTOGENTOO_CMD_LINE__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../portage/tools/_misc_tools.hpp"

using namespace std;

typedef void (*Function)(void);
typedef map<string, Function> script_map;

class CommandLine
{
	public:
	string prompt;
	script_map m;
	
	CommandLine ( string _prompt )
	{
		prompt = _prompt;
	}
	
	void parse_commands ( string _cmd )
	{
		script_map::const_iterator iter = m.find ( _cmd );
		if ( iter != m.end ( ) )
		{
			( *iter->second ) ( );
		}
		else
		{
			cout << "AutoGentoo: command \'" + _cmd + "\' not found" << endl;
		} 
	}
	
	void addOption ( string cmd, void (*f)(void) )
	{
		m.insert ( std::make_pair ( cmd, f ) );
	}
	void start ( string &cmd )
	{
		while ( true )
		{
			cout << prompt;
			cin >> cmd;
			if ( cmd == "exit" or cmd == "quit" or cmd == "q" )
			{
				break;
			}
			parse_commands ( cmd );
			
		}
	}
};

#endif
