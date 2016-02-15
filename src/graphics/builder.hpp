/*
 * main.hpp
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


#ifndef __AUTOGENTOO_GTK_BUILDER__
#define __AUTOGENTOO_GTK_BUILDER__

#include <iostream>
#include <string>
#include <vector>
#include <gtkmm.h>

class builder
{
	public:
	Glib::RefPtr<Gtk::Builder> __builder__;
	Gtk::Window* __main;
	std::string __file;
	std::string __name;
	
	void init ( std::string file, std::string name )
	{
		__file = file;
		__name = name;
		__builder__ = Gtk::Builder::create_from_file ( file.c_str ( ) );
		__builder__->get_widget ( "main", __main );
	}
	
	template < class T >
	T get_widget ( std::string __widget )
	{
		T return_obj;
		__builder__->get_widget ( __widget, return_obj );
		return return_obj;
	}
	
	template < class T >
	T operator [ ] ( std::string __widget )
	{
		return get_widget < T > ( __widget );
	}
};

struct widget
{
	std::vector < builder > _builders_vec;
	std::vector < std::string > _builders_name;
	
	void add_builder ( builder __in )
	{
		_builders_vec.push_back ( __in );
		_builders_name.push_back ( __in.__file );
	}
	
	builder operator [ ] ( std::string __build_str )
	{
		for ( size_t i = 0; i != _builders_name.size ( ); i++ )
		{
			if ( _builders_name [ i ] == __build_str )
			{
				return _builders_vec [ i ];
			}
		}
	}
};
#endif
