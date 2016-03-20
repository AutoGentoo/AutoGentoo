/*
 * type.hpp
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
#include "../package/EmergePackage.hpp"
#include "warning.hpp"

#ifndef __AUTOGENTOO_PORTAGE_TYPE__
#define __AUTOGENTOO_PORTAGE_TYPE__

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
	
	vector<GentooConfig> configs;
	vector<EmergePackage> packages;
	vector<Warning> warnings;
	
	Type ( vector<string> inputFile, string package )
	{
		vector< vector<string> > usegroups;
		vector<string> packageLines;
		vector<string> blocksLines;
		vector< vector<string> > warninggroups;
		for ( size_t y = 0; y != inputFile.size ( ); y++ )
		{
			string line = inputFile[y];
			if ( line.empty ( ) )
			{
				continue;
			}
			string t = findTypes ( line );
			if ( t == "esc" )
			{
				break;
			}
			if ( t == "use" )
			{
				vector<string> useLines;
				size_t b;
				for ( size_t z = y; !inputFile[z].empty ( ); z++ )
				{
					string buff = inputFile[z];
					trim ( buff );
					if ( !buff.empty ( ) )
					{
						useLines.push_back ( inputFile[z] );
					}
					b = z;
				}
				y = b;
				if ( !useLines.empty ( ) )
				{
					usegroups.push_back ( useLines );
				}
			}
			
			if ( t == "package" )
			{
				packageLines.push_back ( line );
			}
			if ( t == "blocks" )
			{
				blocksLines.push_back ( line );
			}
			if ( t == "warning" )
			{
				vector<string> warningLines;
				size_t b = 0;
				for ( size_t z = y; z != inputFile.size ( ); z++ )
				{
					if ( inputFile[z].empty ( ) && inputFile[z+1].empty ( ) )
					{
						b = z;
						break;
					}
					string buff = inputFile[z];
					trim ( buff );
					if ( !buff.empty ( ) or buff == "\n" )
					{
						warningLines.push_back ( buff );
					}
					b = z;
				}
				if ( !warningLines.empty ( ) )
				{
					y = b;
					warninggroups.push_back ( warningLines );
				}
			}
		}
		for ( size_t y = 0; y != usegroups.size ( ); y++ )
		{
			GentooConfig current ( usegroups[y], package );
			configs.push_back ( current );
		}
		for ( size_t y = 0; y != packageLines.size ( ); y++ )
		{
			string buff = packageLines[y];
			trim ( buff );
			EmergePackage current ( buff );
			packages.push_back ( current );
		}
		for ( size_t y = 0; y != blocksLines.size ( ); y++ )
		{
			string buff = blocksLines[y];
			trim ( buff );
			blocks current ( buff );
			string unmerge ( "emerge --rage-clean " + current.blocked );
			system ( unmerge.c_str ( ) );
		}
		for ( size_t y = 0; y != warninggroups.size ( ); y++ )
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
		else if ( line.substr ( 0, 7 ) == string ( "[ebuild" ) )
		{
			return "package";
		}
		else if ( line.substr ( 0, 7 ) == string ( "[blocks" ) )
		{
			return "blocks";
		}
		else if ( line.substr ( 0, 5 ) == string ( "These" ) or line.substr ( 0, 11 ) == string ( "Calculating" ) or line.substr ( 0, 5 ) == string ( "Total" ) or line.substr ( 0, 2 ) == string ( " *" ) )
		{
			return "";
		}
		else if ( line.substr ( 0, 48 ) == "!!! The following installed packages are masked:" )
		{
			return "esc";
		}
		return "warning";
	}
};

#endif
