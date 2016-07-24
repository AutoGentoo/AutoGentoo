/*
 * warning.hh
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


#ifndef __AUTOGENTOO_WARNING___
#define __AUTOGENTOO_WARNING___

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "blocks.hh"
#include "ebuild.hh"
#include "package.hh"

using namespace std;

/*! \class Warning
 * Three types of warnings:
 *    blocks
 *    useReq (edit /etc/portage/make.conf)
 *    genericSlot (do nothing)
 */
class Warning
{
  public:
  
  string type;
  vector < string > input;
  size_t lineNum;
  
  Warning ( vector < string > _input );
  void doWork ( void );
};

#endif
