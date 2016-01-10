/*
 * Emergepackage.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
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
#include <string>

using namespace std;

class Package
{
	public:
	
	string KEYWORDS; //!< This variable now supports a couple of different functions. First of all, this variable specifies what architecture the ebuild is meant for. Some example keywords include: x86, ppc, sparc, mips, alpha, arm, hppa, amd64 and ia64. See the profiles/arch.list file in the Portage tree for more details. Obviously, you would set this to reflect the architecture of the target machine. Portage will not allow an x86 machine to build anything but x86, as specified by the KEYWORDS variable. Packages that do not support the native architecture are automatically masked by Portage. If the KEYWORDS flag has a preceding ~, then that indicates that the particular ebuild works, but needs to be tested in several environments before being moved to the stable profile with the given keyword. If the KEYWORDS flag has a preceding -, then the package does not work with the given keyword. If there is nothing leading KEYWORDS, then the package is considered stable. You can allow installation of these different types of packages through the ACCEPT_KEYWORDS variable in make.conf.
	string IUSE; //!< This is set to whatever USE variables your package utilizes. Remember that KEYWORDS should not be listed in here!
	Package ( string nonAmbigiousName )
	{
		
	}
