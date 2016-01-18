/*
 * ebuild.hpp
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
#include <string>
#include <cstdlib>
#include <map>
#include "../tools/_misc_tools.hpp"
#include "../tools/file.hpp"
#include "../config/parse_config.hpp"
#include "package.hpp"

using namespace std;

typedef struct
{
	public:
	bool enabled;
	bool alreadyInstalledWith;
	string name;
	string description;
	
	void init ( string input )
	{
		/*
		 * + + bluetooth          : Enable Bluetooth Support
		 *  ^ ^         ^         ^                         ^
		*/
		
		vector < int > inputDivide;
		
		for ( size_t charNum; charNum != input.length ( ); charNum++ )
		{
			char currChar = input [ charNum ];
			if ( currChar == ' ' )
			{
				inputDivide.push_back ( charNum );
			}
			
			if ( inputDivide.size ( ) == 3 )
			{
				break;
			}
		}
		
		vector < string > infoVec ( misc::splitByVec ( input, inputDivide ) );
		
		infoVec.push_back ( misc::substr ( input, input.find ( ":" ) + 1, input.length ( ) ) );
		
		enabled = checkEnabled ( infoVec [ 0 ] );
		alreadyInstalledWith = checkEnabled ( infoVec [ 1 ] );
		name = infoVec [ 2 ];
		description = infoVec [ 3 ];
	}
	
	bool checkEnabled ( string chr, bool _default=false )
	{
		if ( chr == "+" )
		{
			return true;
		}
		else if ( chr == "-" )
		{
			return false;
		}
		
		return _default;
	}
} Use;

class ebuild
{
	public:
	string REQUIRED_USE; //!< A list of assertions that must be met by the configuration of Use flags to be valid for this ebuild. (Requires EAPI>=4.)
	vector < string > CURRENT_USE; //!< Found by executing the 'equery uses' command to get the currently enabled strings, this vectors only holds the name of the Use flag
	map < string, Use > USE_MAP; //!< Maps the name of the Use flags 
	
	ebuild ( Package package )
	{
		string sourceCmd ( "source " + package.file + " 2> /dev/null" );
		system ( sourceCmd.c_str ( ) );
		REQUIRED_USE = get_command_str( "echo $REQUIRED_USE" );
		cout << REQUIRED_USE << endl;
		string equeryCmd ( "equery uses " + package.name + " > use" );
		system ( equeryCmd.c_str ( ) );
		
		vector <string > uses_raw ( File ( "use" ).readlines ( ) );
		int startLine ( misc::find < string >  ( uses_raw, " U I" ) );
		
		for ( size_t x = startLine + 1; x != uses_raw.size ( ); x++ )
		{
			Use currUse;
			currUse.init ( uses_raw [ x ] );
			USE_MAP [ currUse.name ] = currUse;
			CURRENT_USE.push_back ( currUse.name );
		}
	}
};
