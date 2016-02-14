/*
* kernel_tools.hpp
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

#ifndef __AUTOGENTOO_KERNEL_TOOLS__
#define __AUTOGENTOO_KERNEL_TOOLS__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector <string> get_indent_content(vector <string> input)
{
	vector<string> return_vec;
	if (input[0][0] == "\t")
	{
		cout << "Inproper syntax! (Tab on first line)" << endl;
		return return_vec;
	}

	for (size_t i = 0; i != input.size(); i++)
	{
		string curr_line(input[i].substr(1, input[i].length() - 1));
		return_vec.push_back(curr_line);
	}

	return return_vec;
}

#endif