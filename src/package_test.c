/*
 * pkg_size.c
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


#include <stdio.h>
#include "package.h"

int main(int argc, char **argv)
{
	mstring str = "[ebuild     U  ] gnome-extra/gnome-shell-extensions-3.18.4::gentoo [3.18.3::gentoo] PYTHON=\"gcc parted\" USE=\"-examples\" ANOTHERONE=\"testing one two three\" 229 KiB";
	Package * gnome = package_new_from_string (str);
	mstring param = gnome->parameters->variables[2];
	
	printf ("%s = %s\n", "ANOTHERONE", map_get (gnome->parameters, param));
	printf ("%s\n", gnome->file);
	fflush (stdout);
	//int __EXIT = package_do_stage (gnome, "compile");
	free(gnome);
	
	return 0;
}

