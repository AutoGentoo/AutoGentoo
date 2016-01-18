/*
 * testUse.cxx
 * 
 * Copyright 2016 Andrei Tumbar <atadmin@Helios>
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


#include <iostream>
#include "../package/ebuild.hpp"
//#include "../package/package.hpp"
//#include "../tools/flagToFile.hpp"

using namespace std;

int main(int argc, char **argv)
{
	Package python ("net-misc/networkmanager-1.0.10::gentoo");
	cout << python.file << endl;
	ebuild py ( python );
	//FlagsToFile(py.CURRENT_USE, py.USE_MAP);
	return 0;
}

