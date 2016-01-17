/*
 * package.hpp
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
#include "../tools/_misc_tools.hpp"
#include "version.hpp"

using namespace std;

class Package
{
	public:
	
	string packagestr;
	string name;
	string path;
	string catagory;
	string directory;
	string releaseStr;
	string file;
	string slot;
	
	version release;
	
	Package ( string input_package )
	{
		/* Key
		 * + added by operator '+'
		 * - subtracted
		 * _ given that it is added by str.substr
		 * ^ found by a str.find() and length ()
		 * * Note added at bottom
		*/
		
		/*
		 * dev-lang/python-2.7.10-r1::gentoo
		 *                          --------
		*/
		misc::remove ( input_package, "::gentoo" );
		
		/*
		 * dev-lang/python-2.7.10-r1
		 * ________^
		*/
		catagory = input_package.substr ( 0, input_package.find ( "/" ) );
		
		/*
		 * dev-lang/python-2.7.10-r1
		 *         ^_____________^
		*/
		name = misc::substr ( input_package, catagory.length ( ) + 1, misc::rfind ( input_package, '-' ) );
		/*
		 * dev-lang/python-2.7.10-r1
		 *         ^________________^
		*/
		string _file = misc::substr ( input_package, catagory.length ( ) + 1, input_package.length ( ) );
		int versionSplit;
		/* 
		 * dev-lang/python-2.7.10-r1
		 *                        ^
		*/
		
		if ( input_package.at ( misc::rfind ( input_package, '-' ) + 1 ) == 'r' )
		{
			/* Has revision
			* python-2.7.10-r1
			* ______^------*
			* Skipped because name = python-2.7.10
			*/
			versionSplit = misc::rfind ( name, '-' );
			name = misc::substr ( name, 0, misc::rfind ( name, '-' ) );
			directory = name.substr ( 0, versionSplit );
		}
		else
		{
			/* No revision
			* python-2.7.10
			*       ^
			*/
			versionSplit = _file.rfind ( "-" );
			directory = name;
		}
		
		path = catagory + "/" + directory;
		/*
		* python-2.7.10-r1
		* ------^_________^
		*/
		releaseStr = misc::substr ( _file, versionSplit + 1, file.length ( ) );
		release.init ( releaseStr );
		cout << name << endl;
		_file = name + "-" + release._in_str;
		
		slot = release.slot;
		
		file = "/usr/portage/" + catagory + "/" + directory + "/" + _file + ".ebuild";
	}
};
