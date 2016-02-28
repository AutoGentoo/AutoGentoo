/*
 * partition.hpp
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

using namespace std;

typedef struct
{
	public:
	string path;
	string fs_type;
	string type; // If in mbr: Primary, Extended, Logical
	string mount_point;
	int size; // In MiB
	int start;
	int end;
	
	void feed ( AutoGentoo_Config &ag_config, string section )
	{
		path = ag_config [ section ] [ "path" ];
		fs_type = ag_config [ section ] [ "fs_type" ];
		type = ag_config [ section ] [ "type" ];
		mount_point = ag_config [ section ] [ "mount_point" ];
		size = ag_config [ section ] [ "size" ];
		start = ag_config [ section ] [ "start" ];
		end = ag_config [ section ] [ "end" ];
	}
} AutoGentoo_Partition;
