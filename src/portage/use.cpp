/*
 * use.hpp
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


#include "use.hh"

useGentooFlag::useGentooFlag ( string package, string uses, bool versionSpecific )
{
  string buffStr ( package + " " + uses + "\n" );
  pkguse.open ( "/etc/portage/package.use" );
  if ( versionSpecific )
  {
    buffStr = "=" + buffStr;
  }
  pkguse << "\n# Use flag (init) for package '" << package << "'" << "\n" << buffStr << endl;
}

void useGentooFlag::append ( string _use )
{
  string buffStr ( package + " " + _use + "\n" );
  pkguse << "\n# Use flag (append) for package '" << package << "'" << "\n" << buffStr << endl;
}
