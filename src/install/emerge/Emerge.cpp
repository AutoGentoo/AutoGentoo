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

using namespace std;
using boost::format;
using boost::io::group;

class Emerge 
{
	public:
	list<string> listPackages[]; 
	string options, defaultOptions, emergeFile;
	char *emergeCommand;
	Emerge(string package,
	string options="", bool do_pretend=true, string emergeFile="emerge.config", string defaultOptions="-q")
	{
		string buffer = str(format("emerge --pretend %s %s %s >> %s") % options % defaultOptions % package % emergeFile);
		emergeCommand = new char[buffer.length() + 1];
		strcpy(emergeCommand, buffer.c_str());
		
		if (do_pretend)
		{
			system(emergeCommand);
		}
		
		options = options;
		defaultOptions = defaultOptions;
		
	}
};


int main(int argc, char **argv)
{
	Emerge test("world", "", false);
	cout << test.emergeCommand << endl;
	return 0;
}
