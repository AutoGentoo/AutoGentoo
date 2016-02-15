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

template < class T >
class __KCONFIG_MODULE__
{
public:
	vector <KERNEL_MODULE> selects;
	vector <NEW_MODULE> depends;
	string type;
	string description;
	T default;

	int feed(vector<string> input)
	{
		if (input[0].substr(0, 6) != "config")
		{
			return 1;
		}

		vector <string> content(get_indent_content(input));
		for (size_t i = 0; i != content.size(); i++)
		{
			string curr_line(content[i]);
			if (curr_line.empty())
			{
				continue;
			}

		}
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