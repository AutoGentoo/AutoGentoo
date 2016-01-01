/*
 * errors.hpp
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
#include <vector>
#include <map>
#include "file.hpp"

using namespace std;

/**\class Type
 * Type class is made to divide section from the 
 * emerge --pretend file and create three different types:
 *    Package
 *    Config
 *    Warning
 * The package lines will be sent to the Package class
 * Config types will be made into a Config instance and sent
 * back to emerge
 * Warning types usually have to do with slot conficts
 * or blocks
 */
class Type
{
	public:
	
	vector<string> inputStrings; //!< Lines inputed from emerge --pretend file
	
	Type ( vector<string> &inputFile, int startLine, int endLine )
	{
		y = startLine;
		for ( x = inputFile[y]; y <= endLine; y++ )
		{
			inputStrings.push_back ( x );
		}
	}
	
	string findTypes ( string line )
	{
		if ( line.substr ( 0, 13 ) == str ( "The following" )
		{
			return "use";
		}
		else if ( line.substr ( 0, 6 ) == str ( "[ebuild" )
		{
			return "package"
		else
		{
			return "warning"
		}
	}
};

