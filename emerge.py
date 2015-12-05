#!/usr/bin/env python
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


import sys, os, subprocess

def get_value(string, start_num, exit_mark=' '):
	curr_char = ""
	temp = ""
	num = start_num
	while curr_char != exit_mark:
		curr_char = string[num]
		temp += curr_char
		num += 1
	temp = temp.replace(exit_mark, "")
	return_val = [temp, num]
	return return_val

class Emerge:
	def __init__(self, package, write_config=True, options="", emerge_file="emerge.config", do_pretend=True, default_options="-q --pretend", remove_config=False):
		if do_pretend:
			# Create the config files to 
			# read and create package and config instances
			# do_pretend is used when emerge new packages without a config given
			pretend_exit = os.system("emerge %s %s %s > %s 2>&1" % (default_options, options, package, emerge_file))
		
		#Setting options
		self.options = options
		self.default_options = default_options
		
		# Variable to read the config file
		self.emerge_file = open(emerge_file, "r").readlines()
		
		if remove_config:
			# Only remove the file if remove_config is True
			os.system("rm -rf %s" % emerge_file) # Remove old file
		
		self.write_config = write_config
		self.main_package = package
		self.packages = {} # package: Package instance
		self.config = {} # file_name: configFile instance
		
		self.getPackages() # Create the package instances
		self.getConfig() # Create the config instances
		
		# Append package paths to the command to
		# later be formated
		self.command = []
		
		for x in self.packages:
			self.command.append("=%s" % self.packages[x].package)
		
		# Convert the self.command list to str
		# with each item split by a ' '
		self.command = ' '.join(self.command)
		
		# Command is the command used to emerge the packages in order 
		self.command = 'emerge -q ' + self.command
	def getPackages(self):
		# Define buffer variables
		self.config_start = []
		self.raw_packages = []
		
		# Config file search for packages
		# Packages have line starting with [ebuild
		# This format is created by the emerge --pretend command
		for x in self.emerge_file:
			if x[1:7] == "ebuild":
				x = x.replace("\n", "")
				self.raw_packages.append(x)
			# Config file changes are detected when the line starts
			# with 'The following' (created by the 'emerge' command)
			if x[0:13] == "The following":
				self.config_start.append(self.emerge_file.index(x))
		for x in self.raw_packages:
			# Create the Package instances and input the package
			# line 
			pkg = Package(x)
			
			# self.packages dictionary is setup with package name : Package instance
			# Ex gnome-base/gnome: Package instace for gnome-base/gnome
			self.packages[pkg.package] = pkg
	def getConfig(self):
		for x in self.config_start:
			curr_line = self.emerge_file[x]
			raw_path_line = self.emerge_file[x+1]
			raw_text = []
			y = x+2
			for y in self.emerge_file[y:]:
				if y != "\n":
					raw_text.append(y)
				else:
					break
			raw_text.insert(0, "# Config for the %s emerge set or package\n" % self.main_package)
			text = ''.join(raw_text)
			file_name = raw_path_line[raw_path_line.index('package'):raw_path_line.index('" ')]
			path = "/etc/portage/%s" % file_name
			self.config[file_name] = configFile(path, text, self.write_config)
	def emerge(self):
		exit_sig = os.system("%s" % self.command)
		return exit_sig
	def emerge_updates(self, execute=True, options=""):
		cmd = []
		for x in self.packages:
			if self.packages[x].updating or self.packages[x].new:
				cmd.append("=%s" % x)
		if len(cmd) == 0:
			updates = False
		else:
			updates = True
		cmd = ' '.join(cmd)
		cmd = "emerge %s %s" % (options, cmd)
		if execute:
			exit_stat = 0
			if updates:
				exit_stat = os.system(cmd)
			print ("No packages left to update")
			return exit_stat
		else:
			print ("Command to be executed %s" % cmd)
	def print_package(self, printAttr=True):
		for x in self.packages:
			print ("Package: %s" % self.packages[x].path)
			print ("	version: %s" % self.packages[x].package)
			if self.packages[x].old:
				print ("	Old package: %s" % self.packages[x].old)
			if not printAttr:
				continue
			for y in self.packages[x].properties:
				print ("	%s: %s" % (y, self.packages[x].properties[y]))
class configFile:
	def __init__(self, path, string="", update_file=True):
		self.path = path
		self.update_text = string
		if not os.path.isdir(path):
			real_path = path
		else:
			real_path = "%s/autogentoo" % path
		if not os.path.exists(real_path):
			os.system("touch %s" % real_path)
		self.file_write = open(path, "a")
		self.file_read = open(path, "r+").readlines()
		
		if self.update_text != "":
			self.has_update = True
		else:
			self.has_update = False
		if update_file:
			self.update()
	def update(self):
		self.file_write.write(self.update_text)
	def new_update(self, string=""):
		self.update_text = string
		if self.update_text != "":
			self.has_update = True
		else:
			self.has_update = False
class Package:
	def __init__(self, package_str):
		self.new = False
		self.slot = False
		self.updating = False
		self.downgrading = False
		self.reinstall = False
		self.replacing = False
		self.fetch_man = False
		self.fetch_auto = False
		self.interactive = False
		self.blocked_man = False
		self.blocked_auto = False
		get_val_list = get_value(package_str, 0, ']')
		get_property = get_val_list[0].replace("[ebuild", "")
		get_property = get_property.replace("]", "")
		self.properties_str = {
			"N": "self.new", #(not yet installed)
			"S": "self.slot", # (side-by-side versions)
			"U": "self.updating", # (to another version)
			"D": "self.downgrading", # (best version seems lower)
			"r": "self.reinstall", # (forced for some reason, possibly due to slot or sub-slot)
			"R": "self.replacing", # (remerging same version)
			"F": "self.fetch_man", # (must be manually downloaded)
			"f": "self.fetch_auto", # (already downloaded)
			"I": "self.interactive", # (requires user input)
			"B": "self.blocked_man", # (unresolved conflict)
			"b": "self.blocked_auto" # (automatically resolved conflict)
		}
		get_val_list = get_value(package_str, 16, ' ')
		self.package = get_val_list[0]
		for x in get_property:
			if x != " ":
				try:
					exec("%s = True" % self.properties_str[x])
				except KeyError:
					pass
		self.properties = {
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
		if self.updating:
			get_val_list = get_value(package_str, get_val_list[1], ']')
			self.old = get_val_list[0].replace("[", "")
			self.old = self.old.replace("]", "")
			self.old = self.old.replace("\n", "")
		else:
			self.old = None
	
	@property
	def path(self):
		search_package = self.package[self.package.index("/")+1:]
		release = search_package[0:search_package.rfind("-")]
		if self.package[-2] == "r":
			path = self.package[0:self.package.index("/")+1] + release[0:release.rfind("-")]
		else:
			path = self.package[0:self.package.index("/")+1] + search_package[0:search_package.rfind("-")]
		return path
def main(args=sys.argv):
	package = "@world"
	emerge_file = "default"
	if len(args) > 1:
		package = args[1]
	if len(args) > 2:
		emerge_file = args[2]
	emerge = Emerge(package, emerge_file=emerge_file)
	emerge.emerge()
if __name__ == '__main__':
	main()
