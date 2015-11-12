xx#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  emerge.py
#  
#  Copyright 2015 Andrei Tumbar <atadmin@Helios>
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


import os, subprocess
from stepPart import get_value

class Emerge:
	def __init__(self, package):
		if not os.path.exists("/usr/portage/%s" % package):
			print ("Package", package, "not found!")
			return None
		pretend_exit = os.system("emerge -q --pretend %s > full_emerge" % package)
		emerge_file = open("full_emerge", "r").readlines()
		errors_start = []
		raw_packages = []
		self.packages = {}
		for x in emerge_file:
			if x[1:7] == "ebuild":
				raw_packages.append(x)
			if x[0:13] = "The following":
				errors_start.append(emerge_file.index(x))
		for x in raw_packages:
			pkg = Package(x)
			self.packages[pkg.path] = pkg
		
class configFile:
	
class Package:
	new = False
	slot = False
	updating = False
	downgrading = False
	reinstall = False
	replacing = False
	fetch_man = False
	fetch_auto = False
	interactive = False
	blocked_man = False
	blocked_auto = False
	def __init__(self, package_str):
		get_val_list = get_value(package_str, 0, ']')
		get_property = get_val_list[0].replace("[ebuild", "")
		get_property = get_property.replace("]", "")
		properties = {
			"N": self.new, #(not yet installed)
			"S": self.slot, # (side-by-side versions)
			"U": self.updating, # (to another version)
			"D": self.downgrading, # (best version seems lower)
			"r": self.reinstall, # (forced for some reason, possibly due to slot or sub-slot)
			"R": self.replacing, # (remerging same version)
			"F": self.fetch_man, # (must be manually downloaded)
			"f": self.fetch_auto, # (already downloaded)
			"I": self.interactive, # (requires user input)
			"B": self.blocked_man, # (unresolved conflict)
			"b": self.blocked_auto # (automatically resolved conflict)
		}
		for x in get_property:
			if x != " ":
				properties[x](True)
		get_val_list = get_value(package_str, 16, ' ')
		self.package = get_val_list[0]
		if self.updating:
			get_val_list = get_value(package_str, get_val_list[1], ']')
			self.update = get_val_list.replace("[", "")
			self.update = self.update("]", "")
	
	@property
	def package(self):
		self.package
	@property
	def path(self):
		self.path = subprocess.check_output("find /usr/portage/* -name '%s.ebuild'" % self.package)
		temp_array = []
		found_dir = False
		for x in self.path:
			if x == "/":
				if found_dir:
					break
				else:
					found_dir = True
			temp_array.append(x)
		return ''.join(temp_array)
	@property
	def new(self, boolean):
		return = bool(boolean)
	@property
	def slot(self, boolean):
		return = bool(boolean)
	@property
	def updating(self, boolean):
		return = bool(boolean)
	@property
	def downgrading(self, boolean):
		return = bool(boolean)
	@property
	def reinstall(self, boolean):
		return = bool(boolean)
	@property
	def fetch_man(self, boolean):
		print ("Manual Fetch for package %s required" % self.path)
		
		return = bool(boolean)
	@property
	def fetch_auto(self, boolean):
		return = bool(boolean)
	@property
	def interactive(self, boolean):
		return = bool(boolean)
	@property
	def blocked_man(self, boolean):
		return = bool(boolean)
	@property
	def blocked_auto(self, boolean):
		return = bool(boolean)
