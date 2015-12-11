/*
 * Emerge.hpp
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
#include <cstring>
#include <list>
#include <boost/format.hpp>
#include "file.hpp"
#include "package.hpp"

using namespace std;
using boost::format;
using boost::io::group;

class Emerge 
{
	public:
	vector<Package> Packages; //!< List of Package classes that was create from the config file
	vector<string> PackageNames, emergeFile;
	vector<int> configStart; //!< Stores the line numbers in emergeFile that need Error creation
	string options, defaultOptions, configFile;
	char *emergeCommand; //!< Executed during emerge ()
	
	Emerge (
	string package,
	string _options="", 
	bool do_pretend=true, 
	const char *configFile="emerge.config", 
	string _defaultOptions="-q"
	)
	{
		string buffer; //!< A string that will be formated to create a char to hold the emerge --pretend command (writes config)
		buffer = str (format("emerge --pretend %s %s %s >> %s") % options % defaultOptions % package % configFile);
		emergeCommand = new char[buffer.length() + 1]; //!< Char with length of the buffer string
		strcpy(emergeCommand, buffer.c_str()); //!< Copy the string into emergeCommand
		
		if (do_pretend)
		{
			/*! Execute the emergeCommand to write the config */
			system(emergeCommand);
		}
		
		options = _options; //!< Options will be used 
		defaultOptions = _defaultOptions;
		emergeFile = File(configFile).readlines();
		getPackages ( );
	}
	void getPackages ( )
	{
		vector<string> rawPackages;
		unsigned int y = 0;
		for ( string x; y < emergeFile.size(); y++ )
		{
			x = emergeFile[y];
			if ( x.length() != 1  && x.length())
			{
				x = x.substr(0, x.length()-1);
			}
			if ( x == string ( "\n" ) || x == string ( " " ) || x == string ( "" ) )
			{
				x = string ("");
			}
			else if (x.substr(1, 6) == string ("ebuild"))
			{
				rawPackages.push_back(x);
			}
			if (x.substr(0, 13) == string("The following"))
			{
				configStart.push_back(strfmt::find<string>(emergeFile, x));
			}
		}
		y = 0;
		for ( string x; y < rawPackages.size(); y++ )
		{
			x = rawPackages[y];
			Package pkg(x.c_str());
			
			Packages.push_back(pkg);
			PackageNames.push_back(pkg.path());
		}
	}
};
