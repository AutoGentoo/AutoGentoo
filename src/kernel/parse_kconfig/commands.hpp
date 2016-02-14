/*
* commands.hpp
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

#ifndef __AUTOGENTOO_KERNEL_OPERATOR__
#define __AUTOGENTOO_KERNEL_OPERATOR__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

string __kernel_command__(string str, bool _indent = false)
{
	vector <string> cmd_list;
	cmd_list.push_back("bool");
	cmd_list.push_back("tristate");
	cmd_list.push_back("string");
	cmd_list.push_back("hex");
	cmd_list.push_back("int");
};

#endif