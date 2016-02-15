#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  builder.py
#  
#  Copyright 2016 Andrei Tumbar <atuser@Kronos>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  


from gi.repository import Gtk, GObject

class __builder ( Gtk.Builder ):
	def __init__ ( self, top_level_file ):
		# Create the page in the step of AutoGentoo
		self.add_from_file ( top_level_file )
		self._file = top_level_file
		self.main = self.get_object ( "main" )
		self.top_level = self.get_object ( "top_level" )
	def __getitem__ ( self, name ):
		return self.get_object ( name )

class __Builder__:
	def __init__ ( self, main_file, main_name ):
		self.bld_set = []
		self._map = {}
		buff = __builder ( main_file )
		self.main_window = buff.main
		self.top_level = buff.top_level
		self.bld_set.append ( buff )
		self._map[main_file] = buff
	def add_builder ( self, _file, name ):
		buff = __builder ( _file )
		self.bld_set.append ( buff )
		self._map[main_name] = buff
	def get_obj ( self, string ):
		exec ( "return self.%s" % string )
	def set_obj ( self, bld, name, obj_name = name ):
		exec ( "self.%s = bld.get_object ( %s )" % ( name, obj_name ) )
	def __getitem__ ( self, __builder_str__ ):
		return self._map [ __builder_str ]
