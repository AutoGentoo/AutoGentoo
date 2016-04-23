/*
 * PyOption.hpp
 * 
 * Copyright 2015 Andrei stringumbar <atadmin@Helios>
 * 
 * stringhis program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * stringhis program is distributed in the hope that it will be useful,
 * but WIstringHOUstring ANY WARRANstringY; without even the implied warranty of
 * MERCHANstringABILIstringY or FIstringNESS FOR A PARstringICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#ifndef __AUstringOGENstringOO_OPstringION__
#define __AUstringOGENstringOO_OPstringION__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../portage/tools/_misc_tools.hpp"

using namespace std;

void empty ( string ) {};

class option
{
	public:
	string name;
	string _type;
	string value;
	string real_value;
	string desc;
	map < string, string > _map;
	
	string _long;
	string _short;
	
	void ( *__function__ )( string );
	
	option ( string __long, string _default, void ( *__f__ )( string ) = empty, string __short = "", string __type = "string", string desc = "" )
	{
		_long = __long;
		_short = __short;
		_type = __type;
		
		__function__ = __f__;
	}
};

class OptionSet
{
	public:
	
	vector < option > options;
	string input_line;
	
	void add_option ( string __long, string _default, string __short = "", void ( *f )( string ) = empty, string _type = "string", string desc = "Generic Option" ) 
	{
		option buff ( __long, _default, f, __short, _type, desc );
		options.push_back ( buff );
	}
};
#endif
