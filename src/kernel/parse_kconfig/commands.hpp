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
#include "../../portage/tools/_misc_tools.hpp"

using namespace std;

struct __KCONFIG_OPERATOR__
{
	vector <string> type_list;
	vector <string> operator_list;
	bool inited;
	void init()
	{
		type_list.push_back("bool");
		type_list.push_back("tristate");
		type_list.push_back("string");
		type_list.push_back("hex");
		type_list.push_back("int");
		type_list.push_back("help");
		operator_list.push_back("=");
		operator_list.push_back("!=");
		operator_list.push_back("!");
		operator_list.push_back("&&");
		operator_list.push_back("||");
		inited = true;
	}

	string operator|= (string compare)
	{
		if (!inited)
		{
			init();
		}
		if (misc::find<string>(type_list, "compare") != -1)
		{
			return "type";
		}
		if (misc::find<string>(operator_list, "compare") != -1)
		{
			return "operator"
		}
		return "variable"
	}
};
#endif