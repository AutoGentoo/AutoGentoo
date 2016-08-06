/*
 * type.hh
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
#include <map>
#include "config.hh"
#include "emergepackage.hh"
#include "_misc_tools.hh"
#include "warning.hh"

#ifndef __AUTOGENTOO_PORTAGE_TYPE__
#define __AUTOGENTOO_PORTAGE_TYPE__

using namespace std;

/**\class Type
 * Type class is made to divide section from the 
 * emerge --pretend file and create three different types:
 *    Package
 *    Config
 *    Warning
 * The package lines will be sent to the Package class
 * Config types will be made into a Config instance and sent
 * back to emerge
 * Warning types usually have to do with slot conficts
 * or blocks
 */
class Type
{
  public:
  
  vector<GentooConfig> configs;
  vector<EmergePackage> packages;
  vector<Warning> warnings;
  
  Type ( vector<string> inputFile, string package, bool updates_only = false );
  string findTypes ( string line, bool up_only = false );
};

#endif
