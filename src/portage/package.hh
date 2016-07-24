/*
 * package.hh
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


#ifndef __AUTOGENTOO_PACKAGE__
#define __AUTOGENTOO_PACKAGE__

#include <iostream>
#include <string>
#include "_misc_tools.hh"
#include "version.hh"

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
  string repository;
  string slot;
  version release;
  string fullPackage;
  string overlayDir;
  
  Package ( string );
  void init ( string );
  int stage ( string, string );
  
};

#endif
