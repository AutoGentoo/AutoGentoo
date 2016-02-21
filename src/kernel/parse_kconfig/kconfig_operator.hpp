/*
* kconfig_operator.hpp
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

typedef struct
{
	vector <string> type_list;
	vector <string> command_list;
	vector <string> operator_list;
	vector <string> skip_list;
	vector <string> misc_list;
	bool inited;
	void init()
	{
		type_list.push_back("bool");
		type_list.push_back("tristate");
		type_list.push_back("string");
		type_list.push_back("hex");
		type_list.push_back("int");
		command_list.push_back("depends");
		command_list.push_back("def_bool");
		command_list.push_back("visible");
		command_list.push_back("def_tristate");
		command_list.push_back("help");
		command_list.push_back("default");
		command_list.push_back("select");
		operator_list.push_back("=");
		operator_list.push_back("!=");
		operator_list.push_back("!");
		operator_list.push_back("&&");
		operator_list.push_back("||");
		skip_list.push_back("on"); 
		misc_list.push_back("defconfig_list");
		misc_list.push_back("modules");
		misc_list.push_back("env");
		misc_list.push_back("allnoconfig_y");
		inited = true;
	}

	string comp (string compare)
	{
		if (!inited)
		{
			init();
		}
		if (misc::find<string>(type_list, compare) != -1)
		{
			return "type";
		}
		if (misc::find<string>(operator_list, compare) != -1)
		{
			return "operator";
		}
		if (misc::find<string>(command_list, compare) != -1)
		{
			return "command";
		}
		if (misc::find<string>(skip_list, compare) != -1)
		{
			return "skip";
		}
		if (misc::find<string>(misc_list, compare) != -1)
		{
			return "misc";
		}
		if (compare == "if")
		{
			return "if";
		}
		return "variable";
	}
} __KCONFIG_OPERATOR__;
#endif
