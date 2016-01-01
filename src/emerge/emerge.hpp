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
#include <boost/format.hpp>
#include "type.hpp"

using namespace std;
using boost::format;
using boost::io::group;

class Emerge 
{
	public:
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
		buffer = str (format("emerge -q --pretend %s >> %s") % package % configFile);
		emergeCommand = new char[buffer.length() + 1]; //!< Char with length of the buffer string
		strcpy(emergeCommand, buffer.c_str()); //!< Copy the string into emergeCommand
		
		if (do_pretend)
		{
			/*! Execute the emergeCommand to write the config */
			system(emergeCommand);
		}
		
		options = _options; //!< Options will be used 
		defaultOptions = _defaultOptions; //!< creates 2 separate inputs for the emerge options such that default options do not have to be re-entered
		emergeFile = File(configFile).readlines();
		Type types(emergeFile, package);
	}
};
