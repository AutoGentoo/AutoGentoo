/*
 * findPackage.hpp
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
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


#include "findPackage.h"

EmergeCat::EmergeCat ( string cat )
{
  string lsCmd ( "ls -1 /usr/portage/" + cat + " >> package" );
  string rmCmd ( "rm -rf package" );
  system ( lsCmd.c_str ( ) );
  vector<string> package_buff = File ( "package" ).readlines ( );
  for ( size_t line = 0; line != package_buff.size ( ); line++ )
  {
    packages.push_back ( package_buff[line] );
  }
  system ( rmCmd.c_str ( ) );
  category = cat;
}

void PortageDir::init ( void )
{
  system ( "ls -1Ad /usr/portage/*/ | grep \"\\-\\|virtual\" > categories" );
  vector<string> cat_buff = File ( "categories" ).readlines ( );
  for ( size_t catnum = 0; catnum != cat_buff.size ( ); catnum++ )
  {
    string remove_dir ( "/usr/portage/" );
    string currDir ( cat_buff[catnum].substr ( remove_dir.length ( ), cat_buff[catnum].length ( ) - remove_dir.length ( ) - 1 ) );
    categories.push_back ( EmergeCat ( currDir ) );
  }
  system ( "rm -rf categories" );
}

string PortageDir::ambig ( string pkg )
{
  for ( size_t currCat = 0; currCat != categories.size ( ); currCat++ )
  {
    for ( size_t currPkgNum = 0; currPkgNum != categories[currCat].packages.size ( ); currPkgNum++ )
    {
      if ( categories[currCat].packages[currPkgNum] == pkg )
      {
        return categories[currCat].category + "/" + categories[currCat].packages[currPkgNum];
      }
    }
  }
  return "Invalid Atom (Not found as full package name in portage tree)";
}
