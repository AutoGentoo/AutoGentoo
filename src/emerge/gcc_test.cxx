/*
 * gcc_test.cxx
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
#include "package.hpp"

using namespace std;

int main(int argc, char **argv)
{
	//EmergePackage gcc ( "[ebuild   R    ] sys-devel/gcc-4.9.3:4.9::gentoo  USE=\"cxx fortran (multilib) nls nptl openmp sanitize (-altivec) (-awt) -cilk -debug -doc (-fixed-point) -gcj -go -graphite (-hardened) (-libssp) (-multislot) -nopie -nossp -objc -objc++ -objc-gc -regression-test -vanilla\" 87,940 KiB" );
	//cout << gcc.flags_str["USE"] << endl;
	//cout << gcc.flags["USE"][1] << endl;
	//cout << gcc.path ( ) << endl;
	//map<string, string> vars = get_command ( "lscpu" );
	string pkg ( argv[1] );
	Package python ( pkg );
	//cout << python.release.revision_num << endl;
	//cout << python.release.revision << endl;
	cout << python.file << endl;
	cout << python.slot << endl;
	//cout << python.revision << endl;
	return 0;
}
