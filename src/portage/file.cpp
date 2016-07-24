/*
* file.hpp
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


#include "file.hh"

File::File (string fileName)
{
  /// Open the file
  file.open ( fileName.c_str ( ) );
  
  /// Send text from file to text
  text << file.rdbuf();
}

/// Return a vector of strings with each element as a line in the file
vector<string> File::readlines ( )
{
  vector <string> returnList ( misc::split ( text.str ( ), '\n', false ) );
  
  /// Return the generated list
  return returnList;
}

string File::read ( )
{
  return text.str ( );
}

/** Return a string of the currentLine in file **/
string File::readline ( )
{
  /// Create a buffer to later be returned
  string buffstr;
  
  vector<string> readFile = readlines ( );
  
  if ( currentLine > readFile.size ( ) )
  {
    /// Set buffstr to line
    buffstr = readFile [ currentLine ];
  }
  
  /// Add one to currentLine
  currentLine++;
  
  /// Return the str
  return buffstr;
}

/** Get a specific line in the file
 * Will use readlines to create an array and then
 * get specific line
*/
string File::get ( int line )
{
  string buffstr; //!< Create a buffer to later be returned
  
  buffstr = readlines ( ) [line]; /// Set buffstr to line
  
  currentLine++; /// Add one to currentLine
  
  /// Return the str
  return buffstr;
}
