/*
 * warning.hpp
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


#ifndef __AUTOGENTOO_WARNING___
#define __AUTOGENTOO_WARNING___

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "../package/blocks.hpp"
#include "../package/ebuild.hpp"
#include "../package/package.hpp"

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
	
	Warning ( vector < string > _input )
	{
		input = _input;
		for ( size_t x = 0; x != input.size ( ); x++ )
		{
			string in = input [ x ];
			if ( in.substr ( 0, 7 ) == string ( "[blocks" ) )
			{
				type = "blocks";
				lineNum = x;
				break;
			}
			else if ( in.substr ( 0, 12 ) == "!!! Problem " )
			{
				type = "useReq";
				lineNum = x;
				break;
			}
			else if ( in.substr ( 0, 12 ) == "!!! Multiple" )
			{
				type = "slot";
				lineNum = x;
				break;
			}
			else
			{
				type = "generic";
				lineNum = x;
			}
		}
		this->doWork ( );
	}
	
	void doWork ( void )
	{
		if ( type == "blocks" )
		{
			blocks b ( input [ lineNum ] );
			string unmerge ( "emerge --unmerge " + b.blocked );
			system ( unmerge.c_str ( ) );
		}
		if ( type == "slot" )
		{
			string pkg = input [ lineNum + 3 ];
			string unmerge ( "emerge --unmerge " + pkg );
			system ( unmerge.c_str ( ) );
		}
		if ( type == "useReq" )
		{
			string buff ( input [ lineNum + 3 ] );
			buff.erase ( 0, 2 );
			Package pkg ( buff.substr ( 0, buff.find ( " " ) ) );
			ebuild pkg_ebuild ( pkg );
		}
	}
};

#endif
