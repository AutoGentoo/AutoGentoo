/*
 * blocks.hpp
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


#include <iostream>
#include <string>

#ifndef __AUTOGENTOO_BLOCKS__
#define __AUTOGENTOO_BLOCKS___

using namespace std;

class blocks
{
	public:
	string blocking;
	string blocked;
	
	blocks ( string input )
	{
		/*
		 * [blocks B ] <sys-apps/portage-2.1.4_rc1 (is blocking app-shells/bash-3.2_p33)
		 * -----------^
		 */
		input.erase ( 0, input.find ( "]" ) + 2 );
		
		/*
		 * <sys-apps/portage-2.1.4_rc1 (is blocking app-shells/bash-3.2_p33)
		 *                            ^
		 */
		blocking = input.substr ( 0, input.find ( " " ) );
		
		/*
		 * <sys-apps/portage-2.1.4_rc1 (is blocking app-shells/bash-3.2_p33)
		 * ---------------------------^
		 */
		input.erase ( 0, blocking.length ( ) + 1 );
		
		/*
		 * (is blocking app-shells/bash-3.2_p33)
		 * -------------
		 */
		input.erase ( 0, string ( "(is blocking " ).length ( ) );
		
		/*
		 * app-shells/bash-3.2_p33)
		 * _______________________-
		 */
		blocked = input.substr ( 0, input.length ( ) - 1 );
	}
};

#endif
