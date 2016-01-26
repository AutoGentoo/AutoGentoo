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


#ifndef __AUTOGENTOO_GTK__BUILDER__
#define __AUTOGENTOO_GTK__BUILDER__

#include <iostream>
#include <string>
#include <gtkmm.h>

using namespace std;

class Builder
{
	public:
	Glib::RefPtr<Gtk::Builder> builder
	Gtk::Window main;
	
	vector < Glib::RefPtr < Gtk::Builder > > bld_set;
	
	Builder ( string file )
	{
		builder = Gtk::Builder::create_from_file ( file.c_str ( ) );
		builder->get_widget ( "main", main );
	}
	
	void get_bld ( string file )
	{
		__builder = Gtk::Builder::create_from_file ( file.c_str ( ) );
		bld_set.push_back ( __builder );
	}
};
#endif
