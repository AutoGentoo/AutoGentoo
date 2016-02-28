/*
 * device.hpp
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
#include "../config/autogentoo_config.hpp"
#include "partition.hpp"

using namespace std;

class AutoGentoo_Device
{
	vector < string > part_list_str;
	vector < AutoGentoo_Partition > part_list;
	AutoGentoo_Config __CFG__;
	
	void read ( string ag_config )
	{
		__CFG__.load ( ag_config );
		vector < string > part_list_str ( __CFG__ ( "device" ) );
		for ( size_t i = 0; i != part_list_str.size ( ); i++ )
		{
			string curr_sec ( part_list_str [ i ] );
			AutoGentoo_Partition curr ( __CFG__, curr_sec );
		}
	}
};
