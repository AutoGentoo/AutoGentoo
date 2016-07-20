/*
 * bash_config.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
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


#include "bash_config.h"

bash_config::bash_config ( const char *_file )
{
  filename = string ( _file );
  file = File(_file).readlines ( );
}

void bash_config::read ( )
{
  size_t y = 0;
  for ( string line = file[y]; y < file.size ( ); y++ )
  {
    if ( line.at ( 0 ) == '#' )
    {
      continue;
    }
    string name = misc::getSubStr( line, 0, '=' );
    misc::remove ( name, " " );
    size_t findOther = line.find ( "$" );
    while ( findOther != string::npos )
    {
      if ( line.at ( findOther+1 ) != '{' )
      {
        cout << format( "Invalid syntax for Config file '%1%', after '$' expected '{' got '%1%' (line %2%)" ) % filename % line.at ( findOther+1 ) % y << endl;
        return;
      }
      string other = misc::getSubStr ( line, findOther, '}' );
      string otherVar = other;
      misc::remove( otherVar, "$" );
      misc::remove( otherVar, "}" );
      misc::remove( otherVar, "{" );
      misc::replace( line, other, otherVar );
      findOther = line.find ( "$", findOther );
    }
    cout << line << endl;
    int equalop = line.find ( "=" );
    string value = line.substr( equalop, line.length ( ) );
    misc::remove(value, "= ");
    misc::remove(value, "=");
    
    variables[name] = value;
  }
}
