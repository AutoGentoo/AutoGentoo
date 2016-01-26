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


#ifndef __AUTOGENTOO_OPTION__
#define __AUTOGENTOO_OPTION__

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include "../portage/tools/_misc_tools.hpp"

using namespace std;

class Option
{
	public:
	//Default variables of an Option
	string _short;
	string _long;
	string _desc;
	vector < vector < string > > arguments;
	vector < vector < string > > argument_values;
	
	Option ( string __short = "", string __long = "", string __desc = "", const vector < vector < string > > _arguments = vector < vector < string > > ( ) )
	{
		if ( __short.find ( "-" ) == string::npos )
		{
			__short = "-" + __short;
		}
		_short = __short;
		if ( !__long.empty ( ) && __long.find ( "--" ) == string::npos )
		{
			__long = "--" + __long;
		}
		_long = __long;
		_desc = __desc;
		arguments = _arguments;
	}
	
	bool operator== ( Option s )
	{
		if ( this->_short == s._short )
		{
			return true;
		}
	}
};

class OptionParser
{
	public:
	
	vector < Option > OptionSet;
	map < string, Option > OptionMap;
	vector < string > programArgs;
	vector < vector < string > > optionVec;
	map < string, string > programArgsMap;
	vector < string > args;
	string programName;
	string programDesc;
	string programHelp;
	
	OptionParser ( string _programName, string _programDesc, vector < string > _programArgs, vector < string > _args )
	{
		args = _args;
		programName = _programName;
		programDesc = _programDesc;
		programArgs = _programArgs;
		for ( size_t i = 0; i != programArgs.size ( ); i++ )
		{
			programArgsMap [ programArgs [ i ] ] = args [ i ];
		}
	}
	
	void setName ( string name )
	{
		programName = name;
	}
	
	void setDesc ( string desc )
	{
		programDesc = desc;
	}
	
	void addOption ( Option newoption )
	{
		OptionMap [ newoption._short ] = newoption;
		if ( !newoption._long.empty ( ) )
		{
			OptionMap [ newoption._long ] = newoption;
		}
		OptionSet.push_back ( newoption );
	}
	
	void setHelp ( string help )
	{
		programHelp = help;
	}
	
	void createHelp ( )
	{
		string lineOne ( programName + ": " + programDesc );
		string usageStr ( programName );
		for ( size_t i = 0; i != programArgs.size ( ); i++ )
		{
			usageStr.append ( " [ " + programArgs [ i ] + " ]" );
		}
		if ( OptionMap.find ( "-h" ) == OptionMap.end ( ) )
		{
			Option h ( "-h", "--help", "Display the help screen and exit" );
			addOption ( h );
		}
		for ( size_t i = 0; i != OptionSet.size ( ); i++ )
		{
			vector < string > buff;
			Option z = OptionSet [ i ];
			if ( z.arguments.size ( ) == 0 )
			{
				buff.push_back ( z._short + " " + z._long );
				buff.push_back ( "" );
				buff.push_back ( z._desc + "\n" );
				optionVec.push_back ( buff );
				continue;
			}
			if ( z.arguments [ i ].size ( ) > 1 )
			{
				buff.push_back ( z._short + " " + z._long );
				buff.push_back ( misc::merge ( z.arguments [ i ], " | " ) );
				buff.push_back ( z._desc + "\n" );
			}
			else if ( z.arguments [ i ].size ( ) == 1 )
			{
				buff.push_back ( z._short + " " + z._long + " " + z.arguments [ i ][ 0 ] );
				buff.push_back ( "" );
				buff.push_back ( z._desc + "\n" );
			}
			else
			{
				buff.push_back ( z._short + " " + z._long );
				buff.push_back ( "" );
				buff.push_back ( z._desc + "\n" );
			}
			optionVec.push_back ( buff );
		}
		programHelp.append ( lineOne + "\n" );
		programHelp.append ( "\n" + string ( "Usage:" ) + "\n" );
		programHelp.append ( usageStr + "\n" );
		programHelp.append ( "\n" + string ( "Options:" ) + "\n" );
	}
	
	void showHelp ( bool small = true )
	{
		cout << programHelp;
		for ( size_t i = 0; i != optionVec.size ( ); i++ )
		{
			string T = optionVec [ i ] [ 1 ];
			if ( optionVec [ i ] [ 1 ].length ( ) > 25 && small )
			{
				vector < string > buff ( misc::split ( optionVec [ i ] [ 1 ], '|', true ) );
				vector < string > after ( buff.begin ( ), buff.begin ( ) + 5 );
				after.push_back ( "..." );
				T = misc::merge ( after, " | " );
			}
			if ( !T.empty ( ) )
			{
				T = " < " + T + " > ";
			}
			cout << setfill(' ') << "  " << setw(100) << left << optionVec [ i ] [ 0 ] + T << setw(30) << left << optionVec [ i ] [ 2 ];
		}
		cout << endl;
	}
	
	bool findOption ( string op )
	{
		vector < string > buff_long;
		vector < string > buff_short;
		for ( size_t i = 0; i != args.size ( ); i++ )
		{
			buff_short.push_back ( OptionMap [ args [ i ] ]._short );
			buff_long.push_back ( OptionMap [ args [ i ] ]._long );
		}
		int found = misc::find < string > ( buff_long, op );
		if ( found != -1 )
		{
			return true;
		}
		found = misc::find < string > ( buff_short, op );
		if ( found != -1 )
		{
			return true;
		}
		return false;
	}
	
	vector < int > get_args ( )
	{
		vector < string > buff;
		for ( size_t i = 0; i != args.size ( ); i++ )
		{
			char first = args [ i ].at ( 0 );
			if ( first == '-' )
			{
				buff.push_back ( 0 );
			}
			else
			{
				buff.push_back ( 1 );
			}
		}
		return buff;
	}
	
	void parse_args ( )
	{
		vector < int > argvec = get_args.size ( );
		vector < string > 
		for ( size_t i = 0; i != argvec.size ( ); i++ )
		{
			int type = argvec [ i ];
			if ( 
			if ( findOption ( j._short ) )
			{
				
};

#endif
