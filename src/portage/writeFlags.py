#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  writeFlags.py
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


import sys

def char_to_bool ( char ):
	if ( char == "+" ):
		return "True"
	return "False"

def main(_file, req):
	_if = open ( _file, "r" ).readlines ( )
	_of = open ( "flags", "w+" )
	_of.write ( "[main]\n" )
	for i in _if:
		enabled = char_to_bool ( i[ 0 ] )
		_of.write ( i[1:-1] + "=" + enabled + "\n" )
	_of.write ("[req]\n")
	_of.write ("required_use=%s" % req)
	return 0

if __name__ == '__main__':
	main(sys.argv[1], sys.argv[2])

