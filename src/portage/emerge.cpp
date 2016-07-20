/*
 * emerge.hpp
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


#include "emerge.h"

void Emerge ( string package, string emergeConfig, string packageConfig, bool do_pretend, string options, bool updates_only )
{
  vector<string> emergeFile;
  
  string emergeCommand ( "emerge --verbose --pretend " + options + " " + package + " > " + emergeConfig + " 2>&1" );
  /*! Execute the emergeCommand to write the config */
  if ( do_pretend )
  {
    system ( emergeCommand.c_str ( ) );
  }
  emergeFile = File ( emergeConfig ).readlines ( );
  Type types ( emergeFile, package, updates_only );
  string trunc ( "truncate -s 0 " + packageConfig );
  system ( trunc.c_str ( ) );
  for ( size_t i = 0; i != types.configs.size ( ); i++)
  {
    GentooConfig curr = types.configs [ i ];
    curr.write ( "" );
  }
  
  for ( size_t i = 0; i != types.packages.size ( ); i++ )
  {
    EmergePackage curr = types.packages [ i ];
    PackageConfig ( curr, packageConfig );
  }
};

