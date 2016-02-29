/*
 * SectionDivide.hpp
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


#ifndef __AUTOGENTOO_SECTION_DIVIDE__
#define __AUTOGENTOO_SECTION_DIVIDE__

#include <iostream>
#include <string>
#include "file.hpp"
#include "_misc_tools.hpp"

using namespace std;

class SectionDivide
{
	public:
	string start_s;
	string end_s;
	string path;
	int start_i;
	int end_i;
	
	SectionDivide ( string _path, string _start_s, string _end_s, int search_num = 0 )
	{
		path = _path;
		start_s = _start_s;
		end_s = _end_s;
		vector <string> __file = File ( path ).readlines ( );
		int curr_g = 0;
		start_i = -1;
		int end_i = -1;
		for ( size_t i = 0; i != __file.size ( ); i++ )
		{
			string curr ( __file [ i ] );
			vector < string > curr_split ( misc::split ( curr, ' ', true ) );
			if ( curr_split [ search_num ] == start_s )
			{
				if ( start_i == -1 )
				{
					start_i = i;
				}
				curr_g++;
			}
			
			if ( curr_split [ search_num ] == end_s )
			{
				--curr_g;
				if ( curr_g == -1 )
				{
					end_i = i;
				}
			}
		}
	}
};

#endif
