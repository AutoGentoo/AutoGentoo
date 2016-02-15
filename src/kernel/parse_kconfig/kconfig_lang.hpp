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
		int p_num = 0;
		vector<int> p_vec;
		vector<int> i_vec;
		for (size_t i = 0; i != input.length(); i++)
		{
			char c = input[i];
			if (c == '(')
			{
				p_num++;
				i_vec.push_back(i)
				p_vec.push_back(p_num);
			}
			if (c == ')')
			{
				int open = i_vec.back();
				int close = i;
				cout << open << " " << input[open] << endl;
				cout << close << " " << input[open] << endl;
			}
		}
	}
};

#endif