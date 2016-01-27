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

class EmergeSet:
	def __init__ ( self, package ):
		self.package = package
		self.in_cfg = package + ".emerge"
		self.out_cfg = package + ".cfg"
		os.system ( "cd %s && ../emerge/emerge %s %s %s" % ( os.path.dirname ( os.path.abspath ( __file__ ) ), self.package, self.in_cfg, self.out_cfg ) )

class PackageSet:
	def __init__ ( self, cfg_file, log_dir ):
		curr_dir = os.path.dirname(os.path.abspath(__file__))
		log_dir = curr_dir + "/" + log_dir
		self.config = configparser.ConfigParser ( )
		self.config.read ( cfg_file )
		self.stages = []
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
		os.system ( "mkdir " + log_dir )
		os.chdir ( "/var/tmp/portage" )
		for package in self.config.sections():
			os.system ( "mkdir -p " + log_dir + "/" + package )
			print ( "\n" )
			print ( "emerge: %s" % package )
			for stage in self.stages:
				sys.stdout.write ( stage )
				sys.stdout.write ( "," )
				os.system ( curr_dir + "/package " + self.config[package]["file"] + " " + stage  + " > " + log_dir + "/" + package + "/" + stage + " .log" + " 2>&1" )
		print ( "\n" )

def main ( argv = sys.argv ):
	package = argv [ 1 ]
	try:
		argv [ 2 ]
	except IndexError:
		log = "logs"
	else:
		log = argv [ 2 ]
	current_emg = EmergeSet ( package )
	current_pkg = PackageSet ( current_emg.out_cfg, log )
main ( )
