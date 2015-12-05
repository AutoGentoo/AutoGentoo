/*
 * PyOption.hpp
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


#include <iostream>
#include <string>
#include <vector> // Vectors
#include <algorithm> // find
#include <list>

using namespace std;


template <class any>

class Option
{
	public:
	
	//Default variables of an Option
	char _short;
	vector<string> _argv;
	string _long;
	string _desc;
	bool hasFunc;
	
	Option ( 
	vector<string> argv,
	const char _short,
	const char *_long = "",
	const char *_desc = "",
	any *_func = NULL )
	{
		//Define the Option information
		_short = string ( _short );
		_long = string ( _long );
		_desc = string ( _desc );
		
		//Define the commandline arguments
		_argv = argv;
		
		//Determine if there is a function to execute
		if ( _func )
		{
			hasFunc = true;
		}
		else
		{
			hasFunc = false;
		}
		
		//Find if the option was used
		if ( getUsed ( _short ) || getUsed ( _long ) )
		{
			// Execute the function if their is one
			if ( hasFunc )
			{
				_func ( );
			}
		}
	}
	
	bool getUsed ( any option )
	{
		return find ( _argv.begin ( ), _argv.end ( ), option) != _argv.end ( ) ;
	}
	
	
	
	any _func ( )
	{
		//Empty execute function
		;
	}
};

template <class any>
class OptionWithArgs
{
	public:
	
	char _short;
	vector<string> _argv;
	string _long;
	string _desc;
	bool hasFunc;
	list<any> arguments;
	
	OptionWithArgs (
	vector<string> argv,
	const char *_short,
	list<any> _arguments,
	const char *_long = "",
	const char *_desc = "",
	any *_func = NULL)
	{
		//Define the Option information
		_short = string ( _short );
		_long = string ( _long );
		_desc = string ( _desc );
		
		//Define the commandline arguments
		_argv = argv;
		
		//Determine if there is a function to execute
		if ( _func )
		{
			hasFunc = true;
		}
		else
		{
			hasFunc = false;
		}
		
		//Find if the option was used
		if ( getUsed ( _short ) || getUsed ( _long ) )
		{
			// Execute the function if their is one
			if ( hasFunc )
			{
				_func ( _arguments );
			}
		}
	}
	
	bool getUsed ( )
	{
		if ( find ( _argv.begin ( ), _argv.end ( ), _short ) != _argv.end ( ) )
		{
			return true;
		}
		else if ( find ( _argv.begin ( ), _argv.end ( ), _long ) != _argv.end ( ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	any _func ( any args )
	{
		;
	}
};

template<>
class OptionParser
{
	public:
	
	list<option> OptionSet
