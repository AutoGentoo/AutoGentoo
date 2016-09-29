/*
 * _misc_tools.hh
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


#ifndef __MISC_TOOLS_AUTOGENTOO__
#define __MISC_TOOLS_AUTOGENTOO__

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

/**\namespace misc
 * The strfmt namespace consists of a various collection of tools
 * to format string
 *
 */
namespace misc
{
  void remove ( std::string &removing, std::string toRemove );
  std::string remove_r ( std::string removing, std::string toRemove );
  void removechar ( std::string &removing, char toRemove );
  void rremovechar ( std::string &removing, char toRemove );
  void removeNewLine ( std::string &str );
  void replace ( std::string &replacing, std::string oldstr, std::string newstr );
  std::string getSubStr ( std::string &input, int startIndex, char exitChar );
  int getSubStrInt ( std::string input, int startIndex, const char *exitChar );
  std::string substr ( std::string input, int start, int end );
  int rfind ( std::string input, char find );
  template < class T >
  int find ( std::vector < T > input, T findstr );
  int find_spec ( std::vector < std::string > input, std::string findstr, int start, int end );
  int strfind ( std::string in, char findchar, int startchar );
  std::vector<std::string> split ( std::string str, char chr, bool _trim );
  std::string itos ( int i );
  std::string merge ( std::vector<std::string>, std::string );
  int stoi ( std::string in );
  bool stob ( std::string in );
  std::string getOutput ( std::string command );
  std::vector < std::string > splitByVec ( std::string input, std::vector < int > vec );
  void print_vec ( std::vector < std::string > in );
  template < class T >
  std::vector < T > set_vec ( std::vector < T > in, size_t pos, T val );
  template < class V, class T >
  bool in ( V VEC, T VAL );
  std::string &ltrim(std::string &s);
  std::string &rtrim(std::string &s);
  std::string &trim(std::string &s);
}

#endif
