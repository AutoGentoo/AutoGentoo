/*
 * config.hh
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


#ifndef __AUTOGENTOO_PORTAGE_CONFIG_FILE__
#define __AUTOGENTOO_PORTAGE_CONFIG_FILE__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "_misc_tools.hh"
#include "is_dir.hh"

using namespace std;

class GentooConfig
{
  public:
  
  vector<string> writeLines;
  string path;
  
  GentooConfig ( vector<string> in, string pkgName );
  void write ( string __path );
};

#endif
