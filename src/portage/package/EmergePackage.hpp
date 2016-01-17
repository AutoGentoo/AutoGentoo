/*
 * EmergePackage.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
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
#include <map>
#include <boost/algorithm/string.hpp>
#include "package.hpp"
#include "../config/parse_config.hpp"

using namespace std;
using namespace boost::algorithm;

/*! \struct PackageProperties
 * This type is meant to store the boolean information
 * of a Gentoo ebuild/package
 */ 
typedef struct PackageProperties
{
	bool _new; //!< not yet installed
	bool _slot; //!< side-by-side versions
	bool _updating; //!< update to another version
	bool _downgrading; //!< best version seems lower
	bool _reinstall; //!< forced for some reason, possibly due to slot or sub-slot
	bool _replacing; //!< remerging same version
	bool _fetchman; //!< must be manually downloaded
	bool _fetchauto; //!< already downloaded
	bool _interactive; //!< requires user input
	bool _blockedman; //!< unresolved conflict
	bool _blockedauto; //!< automatically resolved conflict
	
	bool createdList; //!< Tells set and the [] operator whether to init the list
	map<string, bool> attrMap;
	map<string, string> packageVar;
	vector<string> varNames;
	
	void createList ( void )
	{
		attrMap["new"] = _new;
		attrMap["slot"] = _slot;
		attrMap["updating"] = _updating;
		attrMap["downgrading"] = _downgrading;
		attrMap["reinstall"] = _reinstall;
		attrMap["replacing"] = _replacing;
		attrMap["fetch_man"] = _fetchman;
		attrMap["fetch_auto"] = _fetchauto;
		attrMap["interactive"] = _interactive;
		attrMap["blocked_man"] = _blockedman;
		attrMap["blocked_auto"] = _blockedauto;
		createdList = true;
	}
	void set ( char in, bool val )
	{
		if ( !createdList )
		{
			createList ( );
		}
		switch ( in )
		{
			case 'N':
				_new = val;
				return;
			case 'S':
				_slot = val;
				return;
			case 'U':
				_updating = val;
				return;
			case 'D':
				_downgrading = val;
				return;
			case 'r':
				_reinstall = val;
				return;
			case 'R':
				_replacing = val;
				return;
			case 'F':
				_fetchman = val;
				return;
			case 'f':
				_fetchauto = val;
				return;
			case 'I':
				_interactive = val;
				return;
			case 'B':
				_blockedman = val;
				return;
			case 'b':
				_blockedauto = val;
				return;
		}
		attrMap[string(1, in)] = val;
	}
	bool operator [] ( string in )
	{
		if ( !createdList )
		{
			createList ( );
		}
		if ( in.length() == 1 )
		{
			char inc = in.at(0);
			switch ( inc )
			{
				case 'N':
					return _new;
				case 'S':
					return _slot;
				case 'U':
					return _updating;
				case 'D':
					return _downgrading;
				case 'r':
					return _reinstall;
				case 'R':
					return _replacing;
				case 'F':
					return _fetchman;
				case 'f':
					return _fetchauto;
				case 'I':
					return _interactive;
				case 'B':
					return _blockedman;
				case 'b':
					return _blockedauto;
			}
		}
		return attrMap[in];
	}
	void addVar ( string in )
	{
		int findOperator = in.find ( "=" );
		
		string varName = in.substr ( 0, findOperator );
		varNames.push_back ( varName );
		
		string value = in.substr ( findOperator, in.length ( ) );
		packageVar[varName] = value;
	}
}PackageProperties;

class EmergePackage: public Package
{
	public:
	
	PackageProperties properties;
	string propertystr;
	string old;
	string slot;
	int sizeOfDownload;
	map< string, vector<string> > flags;
	map< string, string > flags_str;
	
	EmergePackage ( const char *input ) : Package ( packagestr )
	{
		/* Key
		 * + added by operator '+'
		 * - subtracted
		 * _ given that it is added by str.substr
		 * ^ found by a str.find() and length ()
		 * * Note added at bottom
		*/
		
		/* [ebuild   R    ] gnome-base/gnome-3.16.0:2.0::gentoo  USE="bluetooth cdr classic cups extras -accessibility" 0 KiB */
		string packageString = string ( input );
		
		
		propertystr = packageString.substr ( 0, 15 );
		
		/* [ebuild   R    ]
		 * -------        -
		 */
		misc::remove ( propertystr, "[ebuild" );
		misc::remove ( propertystr, "]" );
		/* |   R    |*/
		char x;
		for ( size_t i = 0; i != propertystr.length ( ); i++ )
		{
			x = propertystr[i];
			properties.set ( x, true );
		}
		
	}
	
	vector < string > splitEbuild ( string input )
	{
		/*
		 * 1. propertySection
		 * 2. _packageStr
		 * 3. oldStr
		 * 4. vars
		 * 5. size
		*/
		vector < string > returnVec;
		
		/* [ebuild     U  ] www-client/firefox-38.5.0::gentoo [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * ________________
		 */
		string propertySection = input.substr ( 0, 15 );
		
		/* [ebuild     U  ] www-client/firefox-38.5.0::gentoo [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * -----------------
		 */
		string rawSection = misc::substr ( input, 16, input.length ( ) ); 
		
		/* www-client/firefox-38.5.0::gentoo [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * _________________________________^
		 */
		string _packageStr = misc::substr ( rawSection, 0, rawSection.find ( " " ) - 1 );
		
		/* www-client/firefox-38.5.0::gentoo [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * ----------------------------------
		 */
		rawSection = misc::substr ( rawSection, packagestr.length ( ) + 1, rawSection.length ( ) );
		
		/* [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * ________________^
		*/
		string oldStr = misc::substr ( rawSection, 0, rawSection.find ( " " ) - 1 );
		
		/* [38.4.0::gentoo] USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 * -----------------
		*/
		rawSection = misc::substr ( rawSection, oldStr.length ( ), rawSection.length ( ) );
		
		/* USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127 KiB
		 *                                                                 ----
		*/
		rawSection.erase ( rawSection.end ( ) - 4, rawSection.end ( ) );
		
		/* USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127
		 *                                                         ^
		*/
		int varToSizeSplit = rawSection.rfind ( " " );
		
		/* USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127
		 * ________________________________________________________^
		*/
		string vars ( misc::substr ( rawSection, 0, varToSizeSplit ) );
		
		/* USE=" USE FLAGS (lots) " LINGUAS="... ( all languages )" 177,127
		 *                                                         ^_______
		*/
		string size ( misc::substr ( rawSection, vars.length ( ) + 1, rawSection.length ( ) ) );
		
		returnVec.push_back ( propertySection );
		returnVec.push_back ( _packageStr );
		returnVec.push_back ( oldStr );
		returnVec.push_back ( vars );
		returnVec.push_back ( size );
		
		return returnVec;
	}
};
