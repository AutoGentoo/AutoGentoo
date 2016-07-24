/*
 * packageConfig.cxx
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


#include "packageConfig.hh"

using namespace std;

void PackageConfig ( EmergePackage pkg, string name )
{
  ofstream file;
  file.open ( name.c_str ( ), ios::app );
  
  string lineOne ( "[" + pkg.path + ":" + pkg.slot + "]" + "\n" );
  file << lineOne;
  
  string _version ( "version=\"" + misc::merge( pkg.release.v_str, "." ) );
  if ( pkg.release.has_revision )
  {
    _version += "-" + pkg.release.revision;
  }
  _version += "\"\n";
  file << _version;
  
  string _name ( "name=\"" + pkg.name + "\"\n" );
  file << _name;
  
  string _cat ( "catagory=\"" + pkg.catagory + "\"\n" );
  file << _cat;
  
  string _slot ( "slot=\"" + pkg.slot + "\"\n" );
  file << _slot;
  vector < string > keys;
  for ( size_t i = 0; i != pkg.properties.attrVec.size ( ); i++ )
  {
    string z = pkg.properties.attrVec [ i ];
    pkg.properties.init ( );
    if ( pkg.properties.attrMap[ z ] )
    {
      keys.push_back ( z );
    }
  }
  string _keys ( "keys=[" + misc::merge ( keys, "," ) + "]\n" );
  file << _keys;
  
  string _file ( "file=\"" + pkg.file + "\"\n" );
  file << _file;
  
  if ( pkg.old._contructed )
  {
    string _version_old ( "old=\"" + misc::merge ( pkg.old.v_str, "." ) );
    if ( pkg.old.has_revision )
    {
      _version_old += "-" + pkg.old.revision;
    }
    if ( _version_old != "old=\"" )
    {
      _version_old += "\"\n";
      file << _version_old;
    }
  }
  
  string _input ( "input=\"" + pkg.input + "\"\n" );
  file << _input;
  
  for ( map < string ,string >::iterator iter = pkg.flags_str.begin ( ); iter != pkg.flags_str.end ( ); iter++ )
  {
    string buff ( iter->first + "=\"" + iter->second + "\"\n" );
    file << buff;
  }
  
  string ENV ( "ENV=[" );
  vector < string > buff;
  
  for ( map < string ,string >::iterator iter = pkg.flags_str.begin ( ); iter != pkg.flags_str.end ( ); iter++ )
  {
    buff.push_back ( iter->first );
  }
  ENV += misc::merge ( buff, "," ) + "]\n";
  
  file << ENV;
}
