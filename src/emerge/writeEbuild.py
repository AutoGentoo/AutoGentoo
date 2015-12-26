#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  writeEbuild.py
#  
#  Copyright 2015 Andrei Tumbar <atuser@Kronos>
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

import configparser

global make_globals
make_globals = configparser.ConfigParser()
make_globals._interpolation = configparser.ExtendedInterpolation()

global make
make = configparser.ConfigParser()
make._interpolation = configparser.ExtendedInterpolation()

def ConfigForMakeGlobal(in_usr=True):
	if in_usr:
		cfg_read = open("/usr/share/portage/config/make.globals", "r").readlines()
	else:
		cfg_read = open("make.globals", "r").readlines()
	cfg = open("make.globals", "w+")
	cfg_read.insert(0, "[main]\n")
	for x in cfg_read:
		cfg.write(x)
	cfg.close()

def ConfigForMake(in_etc=True):
	if in_etc:
		cfg_read = open("/etc/portage/make.conf", "r").readlines()
	else:
		cfg_read = open("make.conf", "r").readlines()
	cfg = open("make.conf", "w+")
	cfg_read.insert(0, "[main]\n")
	for x in cfg_read:
		cfg.write(x)
	cfg.close()

def ReadGlobals():
	try:
		make_globals.read('make.globals')
	except configparser.DuplicateOptionError:
		pass

def Read():
	make_globals.read('make')

def main():
	ReadGlobals()
	#Read()
	for x in make_globals["main"]: 
		print (x)
		print (make_globals["main"][x])
	return 0

if __name__ == '__main__':
	main()

