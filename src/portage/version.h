/*
 * version.h
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


#include <iostream>
#include <string>
#include <vector>
#include "_misc_tools.h"

#ifndef __GENTOO__PACKAGE_VERSION__
#define __GENTOO__PACKAGE_VERSION__

using namespace std;

class version
{
  public:
  vector<int> v; //!< Vector of the version, string divided by '.' for example 1.2.3 = [1, 2, 3]
  vector<string> v_str; //!< Vector of the version, string divided by '.' for example 1.2.3 = ["1", "2", "3"]
  string revision;
  string slot;
  int revision_num;
  bool has_revision;
  string _in_str;
  bool _contructed;
  
  void init ( string in_str );
  bool operator < ( version compare );
  bool operator <= ( version compare );
  bool operator >= ( version compare );
  bool operator > ( version compare );
  bool operator = ( version compare );
};

#endif
