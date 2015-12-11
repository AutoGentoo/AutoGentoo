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
#include "file.hpp"

using namespace std;

/**\class Error
 * Error class is made to write config files generated from
 * a section of text from a emerge --pretend
 * 
 * This class will have the following:
 *    bool determining if file will be writen
 *    path to file to write
 *    vector of strings to write to file
 *    vector of ints corresponding to line number to write the string
 *
 */
class Error
{
	public:
	
	bool writeFile;
	string fileToWrite;
	vector<string> stringsToWrite;
	vector<int> lineNumbers;
	
	Error ( vector<string> &inputFile, int startLine, int endLine )
	{
		vector<string>::const_iterator first; //!< Will be set to the first line in the input
		vector<string>::const_iterator last; //!< Will be set to the last line in the input
		
		first = inputFile.begin() + startLine;
		last = inputFile.begin() + endLine;
		vector<string> newVec(first, last); //!< Create a vector of strings with block
	}
	
	
};
