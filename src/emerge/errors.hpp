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
	
	bool writeFile; //!< Write file?
	string fileToWrite; //!< Path to the write
	
	vector<string> stringsToWrite; //!< Strings to write, each element corresponds to the lineNumbers vector
	vector<string> inputStrings; //!< Strings define in the INIT function
	
	vector<int> lineNumbers; //!< Line numbers in the file to write
	
	Error ( vector<string> &inputFile, int startLine, int endLine )
	{
		y = startLine;
		for ( x = inputFile[y]; y <= endLine; y++ )
		{
			inputStrings.push_back ( x );
		}
		
		
	}
	
	void findType (  )
	{
		string firstLine = inputString[0];
		
	void addTypes ( map<string, string> &in )
	{
		in["The following"] = "use";
		
};
