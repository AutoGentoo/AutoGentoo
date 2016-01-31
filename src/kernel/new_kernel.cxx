/*
 * new_kernel.cxx
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


#ifndef __AUTOGENTOO_NEW_KERNEL__
#define __AUTOGENTOO_NEW_KERNEL__

#include <iostream>
#include <string>
#include <vector>
#include "kernel.hpp"
#include "new_module.hpp"

using namespace std;

class NewKernel
{
	public:
	string input;
	string ouput;
	string arch;
	kernel __kernel;
	
	NewKernel ( kernel _kernel, string _arch="amd64" )
	{
		arch = _arch;
		__kernel = _kernel;
	}
	
	void _change_mods ( vector < NEW_MODULE > __new_vec )
	{
		for ( size_t i = 0; i != __new_vec.size ( ); i++ )
		{
			this->_change_mod ( __new_vec [ i ], false );
		}
		__kernel.write ( );
	}
	
	void _change_mod ( NEW_MODULE __new, bool write = true )
	{
		__kernel.set_module ( __new.name, __new.changed_value );
		if ( write )
		{
			__kernel.write ( );
		}
	}
	
	void _change_mod_str ( NEW_MODULE __new, string __str, bool write = true )
	{
		
};
#endif
