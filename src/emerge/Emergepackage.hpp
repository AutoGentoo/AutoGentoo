/*
 * Emergepackage.hpp
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
#include "parse_config.hpp"

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
		
		/* [ebuild   R    ] gnome-base/gnome-3.16.0:2.0::gentoo  USE="bluetooth cdr classic cups extras -accessibility" 0 KiB
		 * ________________
		 */
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
		
		/* [ebuild   R    ] gnome-base/gnome-3.16.0:2.0::gentoo  USE="bluetooth cdr classic cups extras -accessibility" 0 KiB
		 * -----------------_________________________________________________________________________________________________
		 */
		rawPackageStr = misc::substr ( packageString, 16, packageString.length ( ) );
		
		vector < string > 
		
		/* gnome-base/gnome-3.16.0:2.0::gentoo  USE="bluetooth cdr classic cups extras -accessibility" 0 KiB
		 * ___________________________________^^
		 */
		packagestr = 
		
		/* gnome-base/gnome-3.16.0:2.0::gentoo
		 *                            --------
		 */
		misc::remove ( packagestr, "::gentoo" );
		
		/* gnome-base/gnome-3.16.0:2.0
		 *                        ^
		 */
		int slotDivide = packagestr.find ( ":" );
		
		/* Has a given slot */
		if ( slotDivide != string::npos )
		{
			/* gnome-base/gnome-3.16.0:2.0
			*                          ___
			*/
			slot = misc::substr ( packagestr, slotDivide + 1, packagestr.length ( ) );
			
			/* gnome-base/gnome-3.16.0:2.0
			*  _______________________----
			*/
			packagestr = misc::substr ( packagestr, 0, slotDivide );
		}
	}
	
	vector < string > splitEbuild ( string input )
	{
		vector < string > returnVec;
		
		//[ebuild     U  ] net-dialup/rp-pppoe-3.11-r3::gentoo [3.8-r2::gentoo] USE="-tk% (-X%*)" 219 KiB
		propertySection = input.substr ( 0, 15 );
		returnVec ( propertySection );
		
		rawSection = misc::substr ( input, 16, input.length ( ) ); 
		
		int firstSecDivide;
		int secondSecDivide;
		int thirdSecDivide;
		bool insideQuote = false;
		for ( size_t i = 0; i != input.length ( ); i++ )
		{
			char j = input[j];
			if ( j == ' ' && !insideQuote )
			{
				switch ( returnVec.size ( ) )
				{
					case 1:
					firstSecDivide = i;
					returnVec push_back ( misc::substr ( input, 16, i - 1 ) );
					case 2:
					secondSecDivide = i;
					returnVec push_back ( misc::substr ( input, firstSecDivide, i ) );
					case 3;
					
				}
				
			}
			if ( j == '\"' )
			{
				insideQuote = !insideQuote;
			}
};
