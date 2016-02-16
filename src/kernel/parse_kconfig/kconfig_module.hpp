/*
* kconfig_module.hpp
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

#ifndef __AUTOGENTOO_KCONFIG_MODULE__
#define __AUTOGENTOO_KCONFIG_MODULE__

#include <iostream>
#include <string>
#include <vector>
#include "../kernel.hpp"
#include "kernel_tools.hpp"

using namespace std;

bool str_to_bool(string input, bool __default=false)
{
	if (input == "y" || input == "true" || input == "True")
	{
		return true;
	}
	if (input == "n" || input == "false" || input == "False")
	{
		return false;
	}
	return __default;
}


class __KCONFIG_MODULE__
{
public:
	vector <KERNEL_MODULE> selects;
	vector <NEW_MODULE> depends;
	string type;
	string prompt;
	string help;
	string value;
	bool __bool__;

	int feed(vector<string> input)
	{
		if (input[0].substr(0, 6) != "config")
		{
			return 1;
		}

		vector <string> content(get_indent_content(input));
		for (size_t i = 0; i != content.size(); i++)
		{
			string curr_line_buff(content[i]);
			if (curr_line_buff.empty())
			{
				continue;
			}
			__KCONFIG_LINE__ curr(curr_line_buff);
			if (!curr.exec_line)
			{
				continue;
			}
			if (curr.keyword.substr(0, 3) == "def")
			{
				type = curr.keyword.substr(3, curr.keyword.length() - 3);
				value = curr.split[1];
				if (type == "bool" || type == "tristate")
				{
					__bool__ = str_to_bool(value);
				}
			}
			else if (curr.is_type)
			{
				type = curr.keyword;
				if (curr.has_prompt)
				{
					prompt = curr.split[1];
				}
			}
			int buff;
			if (curr.keyword == "help")
			{
				for (size_t j = i; i != content.size(); j++)
				{
					string check_line(content[j]);
					if (check_line[0] != '\t')
					{
						buff = j;
					}
					else
					{
						break;
					}
				}
			}
			vector <string> buff_vec(input.begin() + i, input.begin() + buff);
			help(vector_to_string(buff_vec));
		}
	}
};
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
	vector <string> split;
	__KCONFIG_LINE__(string input)
	{
		vector < string > __split(misc::split(input, ' ', true));
		for (size_t i = 0; i != __split.size(); i++)
		{
			 __keyword = __KCONFIG_OPERATOR__ |= __split[i];
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

			}
		}
		keyword = __split[0];
		split = __split;
	}
};

/*Example __KCONFIG_MODULE
config GENTOO_LINUX_UDEV
	bool "Linux dynamic and persistent device naming (userspace devfs) support"
	
	depends on GENTOO_LINUX
	default y if GENTOO_LINUX
	
	select DEVTMPFS
	select TMPFS
	
	select MMU
	select SHMEM
	
	help
		In order to boot Gentoo Linux a minimal set of config settings needs to
		be enabled in the kernel; to avoid the users from having to enable them
		manually as part of a Gentoo Linux installation or a new clean config,
		we enable these config settings by default for convenience.
		
		Currently this only selects TMPFS, DEVTMPFS and their dependencies.
		TMPFS is enabled to maintain a tmpfs file system at /dev/shm, /run and
		/sys/fs/cgroup; DEVTMPFS to maintain a devtmpfs file system at /dev.
		
		Some of these are critical files that need to be available early in the
		boot process; if not available, it causes sysfs and udev to malfunction.
		
		To ensure Gentoo Linux boots, it is best to leave this setting enabled;
		if you run a custom setup, you could consider whether to disable this.
*/