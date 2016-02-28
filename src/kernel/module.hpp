/*
* module.hpp
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

#ifndef __AUTOGENTOO_MODULE__
#define __AUTOGENTOO_MODULE__

#include <iostream>
#include <string>
#include <vector>
#include "kernel_module.hpp"
#include "parse_kconfig/kconfig_module.hpp"

class __MODULE__
{
public:
	string NAME;
	string _MOD_TYPE;
	string TYPE;
	string VALUE;
	string HELP;
	string PROMPT;
	vector<string> DEPENDS;
	vector<string> SELECTS;
	bool BOOL;
	__KCONFIG_MODULE__ kconfig_module;
	KERNEL_MODULE kernel_module;
	__MODULE__ ( __KCONFIG_MODULE__ module )
	{
		_MOD_TYPE = "kconfig";
		NAME = module.name;
		TYPE = module.type;
		VALUE = module.value;
		HELP = module.help;
		PROMPT = module.prompt;
		DEPENDS = module.depends;
		SELECTS = module.selects;
		kconfig_module = module;
	}
	__MODULE__ ( KERNEL_MODULE module )
	{
		_MOD_TYPE = "kernel";
		NAME = module.name;
		TYPE = module.type;
		VALUE = module.value;
		if (TYPE == "bool")
		{
			BOOL = module.value_bool;
		}
		kernel_module = module;
	}
};

#endif
