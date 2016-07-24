/*
 * is_dir.hpp
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



#include "is_dir.hh"

bool is_dir ( const char* path )
{
  struct stat st_buf;
  
  int status = stat ( path, &st_buf );
  
  if ( status != 0 )
  {
    throw;
  }
  
  if ( S_ISREG ( st_buf.st_mode ) )
  {
    return false;
  }
  else if ( S_ISDIR ( st_buf.st_mode ) )
  {
    return true;
  }
  return false;
}

bool is_file_exist ( const char *fileName )
{
  std::ifstream infile ( fileName );
  return infile.good ( );
}
