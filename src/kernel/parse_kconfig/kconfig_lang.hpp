/*
* kconfig_lang.hpp
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

#ifndef __AUTOGENTOO_KERNEL_LANGUAGE__
#define __AUTOGENTOO_KERNEL_LANGUAGE__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "commands.hpp"

using namespace std;

class __Kconfig_Set__
{
public:
	string input_str;
	map < int, int > open_to_close;
	map < int, int > close_to_open;
	vector < int > open_vec;
	vector < int > close_vec;

	__Kconfig_Set__(string input)
	{
		input_str = input;
		vector<int> i_vec;
		for (size_t i = 0; i != input.length(); i++)
		{
			char c = input[i];
			if (c == '(')
			{
				i_vec.push_back(i)
			}
			if (c == ')')
			{
				int open = i_vec.back();
				i_vec.pop_back();
				int close = i;
				cout << open << " " << input[open] << endl;
				cout << close << " " << input[close] << endl;
				open_vec.push_back(open);
				close_vec.push_back(close);
				open_to_close[open] = close;
				close_to_open[close] = open;
			}
		}
	}
};

bool 

bool __Kconfig_if__(string input, kernel &__kernel__)
{
	__Kconfig_Set__ group(input);
	vector < string > __split(misc::split(input, ' ', true));
	__Kconfig_Set__ __group(input);
	
	for (size_t i = 0; i != __group.open_vec.size(); i++)
	{
		int open = __group.open_vec[i];
		int close = __group.close_vec[i];
	}
	
	/*X86_64 || (X86_32 && HIGHMEM)*/
}
#endif
