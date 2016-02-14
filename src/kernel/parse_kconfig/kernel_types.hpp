/*
* kernel_types.hpp
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

#ifndef __AUTOGENTOO_KERNEL_TYPES__
#define __AUTOGENTOO_KERNEL_TYPES__

#include <iostream>
#include <string>

using namespace std;

template < class T >
class kernel_type
{
public:
	string type_str;
	T val_str;
};

class kernel_string: public kernel_type <string>
{
public:
	kernel_string(string value)
	{
		val_str = value;
		type_str = "string"
	}
};

class kernel_int : public kernel_type <int>
{
public:
	kernel_int(int value)
	{
		val_str = value;
		type_str = "int"
	}
};

class kernel_bool : public kernel_type <bool>
{
public:
	kernel_bool(bool value)
	{
		val_str = value;
		type_str = "bool"
	}
};

#endif