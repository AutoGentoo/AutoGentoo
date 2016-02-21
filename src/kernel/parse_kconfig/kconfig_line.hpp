/*
 * kconfig_line.hpp
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

#ifndef __AUTOGENTOO_KCONFIG_LINE__
#define __AUTOGENTOO_KCONFIG_LINE__

#include <iostream>
#include "../kernel.hpp"
#include "kernel_tools.hpp"
#include "kconfig_operator.hpp"

class __KCONFIG_LINE__
{
public:
	bool exec_line;
	bool has_if;
	bool is_type;
	bool has_prompt;
	bool is_misc;
	string keyword;
	string __keyword;
	string if_stat;
	vector <string> split;
	__KCONFIG_LINE__(string input)
	{
		vector < string > __split(misc::split(input, ' ', true));
		for (size_t i = 0; i != __split.size(); i++)
		{
			__KCONFIG_OPERATOR__ op_buff;
			__keyword = op_buff.comp ( __split[i] );
			if (__keyword == "type")
			{
				is_type = true;
				if (i + 1 >= __split.size())
				{
					has_prompt = false;
				}
				else
				{
					has_prompt = true;
				}
			}
			if (__keyword == "if")
			{
				has_if = true;
				if_stat = misc::merge ( vector < string > ( __split.begin ( ) + i + 1, __split.end ( ) ), " " );
			}
		}
		keyword = __split[0];
		split = __split;
	}
};

#endif
