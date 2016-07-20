/*
 * file.h
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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


#ifndef __AUTOGENTOO_FILE__
#define __AUTOGENTOO_FILE__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "_misc_tools.h"

using namespace std;

/*! \class File
 *
 * class File is meant to create vector of strings to be used
 * during reading files line by line
 *
 */
class File
{
  public:
  
  /// Empty file constructor
  ifstream file;
  
  /** String with whole file stored affecter File () is
   * called
   */
  stringstream text;
  
  /** Used during readline () function
   * Every time this function is run then currentLine will increase by 1
   */
  size_t currentLine;
  
  File ( string fileName );
  vector<string> readlines ( );
  string read ( );
  string readline ( );
  string get ( int line );
};

#endif
