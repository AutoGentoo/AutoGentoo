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
#include <sstream> // For char to string conversions
#include <map> // Option type definition
#include <vector> // Vectors
#include <algorithm> // find
#include <list>

using namespace std;

class Option
{
	public:
	
	//Default variables of an Option
	char _short;
	vector<string> _argv;
	string _long;
	string _desc;
	
	Option ( 
	vector<string> argv,
	const char short_,
	const char *long_ = "",
	const char *desc = "")
	{
		//Define the Option information
		stringstream buff;
		buff << short_;
		buff >> _short;
		
		buff << long_;
		buff >> _long;
		
		buff << desc;
		buff >> _desc;
		
		//Define the commandline arguments
		_argv = argv;
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
};

class OptionWithArgs
{
	public:
	
	//Default variables of an OptionWithArgs
	char _short;
	vector<string> _argv;
	string _long;
	string _desc;
	list<string> arguments;
	
	OptionWithArgs (
	vector<string> argv,
	const char short_,
	list<string> _arguments,
	const char *long_ = "",
	const char *desc = "")
	{
		//Define the Option information
		stringstream buff;
		buff << short_;
		buff >> _short;
		
		buff << long_;
		buff >> _long;
		
		buff << desc;
		buff >> _desc;
		
		//Define the commandline arguments
		_argv = argv;
	}
};


class OptionParser
{
	public:
	
	list<Option> OptionSet;
	string _programName;
	string _programDesc;
	
	OptionParser (
	string programName,
	string programDesc,
	bool createHelp )
	{
		;
	}
	
	void setName ( const char **name )
	{
		stringstream buff;
		buff << name;
		buff >> _programName;
	}
	
	void setDesc ( const char **desc )
	{
		stringstream buff;
		buff << desc;
		buff >> _programDesc;
	}
	
	void addOption ( Option newoption )
	{
		OptionSet.push_back ( newoption );
	}
};
