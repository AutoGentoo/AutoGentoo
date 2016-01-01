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
#include "config.hpp"
#include "package.hpp"
#include "warning.hpp"

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
	
	vector<Config> configs;
	vector<Package> packages;
	vector<Warning> warnings;
	
	Type ( vector<string> &inputFile, string package )
	{
		vector< vector<string> > usegroups;
		vector<string> packageLines;
		vector< vector<string> > warninggroups;
		for ( unsigned int y; y <= inputFile.size ( ); y++ )
		{
			string line = inputFile[y];
			string t = findTypes ( line );
			if ( t == "use" )
			{
				vector<string> useLines;
				for ( unsigned int z; inputFile[z] != "\n"; z++ )
				{
					useLines.push_back ( inputFile[z] );
					y = z;
				}
				usegroups.push_back ( useLines );
			}
			
			if ( t == "package" )
			{
				packageLines.push_back ( line );
			}
			
			if ( t == "warning" )
			{
				vector<string> warningLines;
				for ( unsigned int z; (inputFile[z] != "\n" && inputFile[z+1] != "\n"); z++ )
				{
					warningLines.push_back ( inputFile[z] );
					y = z;
				}
				warninggroups.push_back ( warningLines );
			}
		}
		
		for ( unsigned int y; y <= usegroups.size(); y++ )
		{
			Config current ( usegroups[y] );
			configs.push_back ( current );
		}
		
		for ( unsigned int y; y <= packageLines.size(); y++ )
		{
			Package current ( packageLines[y].c_str() );
			packages.push_back ( current );
		}
		
		for ( unsigned int y; y <= warninggroups.size(); y++ )
		{
			Warning current ( warninggroups[y] );
			warnings.push_back ( current );
		}
	}
	
	string findTypes ( string line )
	{
		if ( line.substr ( 0, 13 ) == string ( "The following" ) )
		{
			return "use";
		}
		else if ( line.substr ( 0, 6 ) == string ( "[ebuild" ) )
		{
			return "package";
		}
		else
		{
			return "warning";
		}
	}
};

