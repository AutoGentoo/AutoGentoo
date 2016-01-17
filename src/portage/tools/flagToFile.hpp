/*
 * flagToFile.hpp
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
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "../package/ebuild.hpp"

using namespace std;

string booltostring (bool a)
{
	if ( a )
	{
		return "True";
	}
	if ( !a )
	{
		return "False";
	}
}

void FlagsToFile ( vector < string > uses, map < string, use > useMap )
{
	ofstream file ( "flags" );
	file << "[main]\n";
	for ( size_t x = 0; x != uses.size ( ); x++ )
	{
		string currLine ( uses [ x ] + "=" + booltostring ( useMap [ uses [ x ] ].enabled ) + "\n" );
		file << currLine;
	}
};
