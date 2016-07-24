/*
 * emergepackage.hh
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


#ifndef __AUTOGENTOO_EMERGE_PACKAGES__
#define __AUTOGENTOO_EMERGE_PACKAGES__

#include <iostream>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include "package.hh"
#include "parse_config.hh"

using namespace std;
using namespace boost::algorithm;

/*! \struct PackageProperties
 * This type is meant to store the boolean information
 * of a Gentoo ebuild/package
 */ 
class PackageProperties
{
  public:
  bool _new; //!< not yet installed
  bool _slot; //!< side-by-side versions
  bool _updating; //!< update to another version
  bool _downgrading; //!< best version seems lower
  bool _reinstall; //!< forced for some reason, possibly due to slot or sub-slot
  bool _replacing; //!< remerging same version
  bool _fetchman; //!< must be manually downloaded
  bool _fetchauto; //!< already downloaded
  bool _interactive; //!< requires user input
  bool _blockedman; //!< unresolved conflict
  bool _blockedauto; //!< automatically resolved conflict
  string _status; //!< Ex: ~ or +
  bool _uninstall;
  
  bool createdList; //!< Tells set and the [] operator whether to init the list
  map<string, bool> attrMap;
  vector<string> attrVec;
  map<string, string> packageVar;
  vector<string> varNames;
  
  void createList ( void );
  void init ( void );
  void set ( char in, bool val );
  bool operator [] ( string in );
  void addVar ( string in );
};

class EmergePackage: public Package
{
  public:
  
  PackageProperties properties;
  string propertystr;
  version old;
  int sizeOfDownload;
  string input;
  string keyword;
  map < string, vector<string> > flags;
  map < string, string > flags_str;
  
  EmergePackage ( string __input, string _keyword );
  vector < string > splitEbuild ( string input );
};

#endif
