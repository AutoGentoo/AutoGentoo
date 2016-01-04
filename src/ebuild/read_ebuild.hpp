/*
 * read_ebuild.hpp
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


#include <iostream>
#include <string>
#include "../emerge/file.hpp"
#include "_operator.hpp"

using namespace std;

struct ebuild_use
{
	string IUSE = "undefined"; //!< A list of all USE flags (excluding arch flags, but including USE_EXPAND flags) used within the ebuild. See IUSE.
	string REQUIRED_USE = "undefined"; //!< A list of assertions that must be met by the configuration of USE flags to be valid for this ebuild. (Requires EAPI>=4.)
	string PACKAGE_USE = "undefined"; //!< Found in /etc/portage/package.use
	
