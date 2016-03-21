#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  package.py
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

import sys, configparser, os

class color:
	esc_seq = "\x1b["
	end = ( "%s39;49;00m" % ( esc_seq ) )
	red = ( "%s31;01m" % ( esc_seq ) )
	green = ( "%s32;01m" % ( esc_seq ) )
	yellow = ( "%s33;01m" % ( esc_seq ) )
	blue = ( "%s34;01m" % ( esc_seq ) )
	magenta = ( "%s35;01m" % ( esc_seq ) )
	cyan = ( "%s36;01m" % ( esc_seq ) )
	bold = ( "\033[1m" )

class EmergeSet:
	def __init__ ( self, package ):
		self.package = package
		slash = package.find ( "/" )
		if slash !=  -1:
			self.in_cfg = package [ slash: ] + ".emerge"
			self.out_cfg = package [ slash: ] + ".cfg"
		else:
			self.in_cfg = package + ".emerge"
			self.out_cfg = package + ".cfg"
		os.system ( "cd %s && ../emerge/emerge %s %s %s" % ( os.path.dirname ( os.path.abspath ( __file__ ) ), self.package, self.in_cfg, self.out_cfg ) )

class PackageSet:
	def __init__ ( self, cfg_file, log_dir, order = None, misc = False ):
		self.stages = []
		self.create_order ( order )
		curr_dir = os.path.dirname ( os.path.abspath ( __file__ ) )
		log_dir = curr_dir + "/" + log_dir
		self.config = configparser.ConfigParser ( )
		self.config.read ( cfg_file )
		os.system ( "mkdir " + log_dir + " 2> /dev/null" )
		curr = 0
		total = len ( self.config.sections ( ) )
		for package in self.config.sections ( ):
			keys = self.config [ package ] [ keys ].replace ( "[", "" ).replace ( "]", "" )
			if misc:
				if "reinstall" in keys:
					continue
			curr += 1
			os.system ( "mkdir -p " + log_dir + "/" + package )
			for stage in self.stages:
				print ( "\r%s%s%s (%s%s%s of %s%s%s) %s(%s)%s   " % ( color.green, package, color.end, color.yellow, curr, color.end, color.yellow, total, color.end, color.bold, stage, color.end ), end="", flush=True )
				os.system ( curr_dir + "/package " + self.config[package]["file"] + " " + stage  + " > " + log_dir + "/" + package + "/" + stage + " .log" + " 2>&1" )
			print ("")
	def create_order ( self, order = None ):
		if ( order ):
			self.stages = order;
		else:
			self.stages.append ( "fetch" )
			self.stages.append ( "pretend" )
			self.stages.append ( "setup" )
			self.stages.append ( "unpack" )
			self.stages.append ( "prepare" )
			self.stages.append ( "configure" )
			self.stages.append ( "compile" )
			self.stages.append ( "test" )
			self.stages.append ( "install" )
			self.stages.append ( "preinst" )
			self.stages.append ( "postinst" )
			self.stages.append ( "merge" )
			self.stages.append ( "prerm" )
			self.stages.append ( "postrm" )
			self.stages.append ( "cleanrm" )

def main ( argv = sys.argv ):
	package = argv [ 1 ]
	try:
		argv [ 2 ]
	except IndexError:
		log = "logs"
	else:
		log = argv [ 2 ]
	
	try:
		argv [ 3 ]
	except IndexError:
		order = None
	else:
		order = argv [ 3 ]
		if order == "None":
			order = None
		if order == "alt":
			order = ["setup","unpack","prepare","configure","compile","install","preinst","postinst","merge","prerm","postrm","cleanrm"]
	try:
		argv [ 4 ]
	except IndexError:
		do_set = True
	else:
		if argv [ 4 ].lower ( ) == "false":
			do_set = False
		else:
			do_set = True
	try:
		argv [ 5 ];
	except IndexError:
		misc = False
	else:
		if argv [ 5 ].lower ( ) == "true":
			misc = True
		else:
			misc = False
	if do_set:
		current_emg = EmergeSet ( package )
		current_pkg = PackageSet ( current_emg.out_cfg, log, order )
	else:
		current_pkg = PackageSet ( package, log, order )
try:
	main ( )
except KeyboardInterrupt:
	exit ( 127 )
