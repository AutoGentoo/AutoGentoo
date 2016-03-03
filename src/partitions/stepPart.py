#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  partitions.py
#  
#  Copyright 2015 Andrei Tumbar <atadmin@Kronos>
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

from gi.repository import Gtk, Gdk, GObject
from parted import *
import os, subprocess, parted, _ped
class part:
	def __init__(self, disk_path, unit):
		os.system("echo -n 'Clearing old values...'")
		part.partitions = []
		part.mounts = []
		part.fileSystem = []
		part.paths = []
		part.size = []
		part.start = []
		part.end = []
		part.type = []
		part.parent = []
		part.flags = []
		part.extended = None
		os.system("echo 'Done'")
		os.system("echo -ne 'Scanning disk %s...\n'" % disk_path)
		curr_disk = disk.path_to_disk[disk_path]
		curr_device = disk.path_to_device[disk_path]
		#Detect partitions
		partitions = getAll(curr_device)
		part.partitions = partitions
		for x in range(0, len(partitions)): 
			if partitions[x].type == 2:
				part.extended = partitions[x]
		for x in partitions: 
			if x.type in [4, 5]:
				part.paths.append("Free Space")
			else:
				part.paths.append(x.path)
			part.start.append(format_units(float(x.geometry.start), unit))
			part.end.append(format_units(float(x.geometry.end), unit))
			part.type.append(getType(x))
			part.size.append(round(x.getLength(unit), 2))
			if int(x.type) in [1, 5]:
				part.parent.append(part.extended.path)
			else:
				part.parent.append(None)
			if not x.fileSystem:
				part.fileSystem.append(None)
			else:
				part.fileSystem.append(x.fileSystem.type)
			mount_find = os.system("mount | grep '%s '" % x.path)
			if mount_find == 0:
				mount = subprocess.check_output("mount | grep '%s '" % x.path, shell=True).decode(encoding="utf-8", errors="strict")
				mount = mount.replace(x.path, "")
				mount = mount.replace(" on ", "")
				part.mounts.append(get_value(mount, 0)[0])
			else:
				part.mounts.append(" ")
			if x.type not in [4, 5]:
				part.flags.append(x.getFlagsAsString())
			else:
				part.flags.append(" ")
def getType(part):
	partTypes = {
		0: "primary",
		1: "logical",
		2: "extended",
		4: "free_primary",
		5: "free_logical"}
	return partTypes[int(part.type)]
def getAll(device):
	partitions = []
	disk = Disk(device)
	disk.__disk = _ped.Disk(device.getPedDevice())
	part = disk.__disk.next_partition()
	while part:
		if part.type & parted.PARTITION_FREESPACE or \
			parted.PARTITION_FREESPACE or \
			parted.PARTITION_METADATA or \
			parted.PARTITION_PROTECTED:
				if int(part.type) not in [8, 9]:
					partitions.append(parted.Partition(disk=disk, PedPartition=part))
		part = disk.__disk.next_partition(part)
	return partitions
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
def format_units(input_float, unit, sector_size=512, demical=2):
	unit = unit.lower()
	input_float = input_float*sector_size
	unit = unit.replace("ib", "")
	unit_list = ["k", "m", "g", "t"]
	unit_values = [1024, 1048576, 1073741824, 1099511627776]
	if unit in unit_list:
		listnum = unit_list.index(unit)
		temp_float = float(input_float)
		temp_float = temp_float/unit_values[listnum]
		ouput_float = str(round(temp_float, demical))
		return ouput_float
def get_iter(gtk_treestore, iters, string, column=0):
	if string == None:
		return None
	current_row = 0
	while gtk_treestore.get_value(iters[current_row], column) != string:
		current_row += 1
	return gtk_treestore.get_iter(gtk_treestore.get_path(iters[current_row]))
def get_difference(old_list, new_list):
	return_list = []
	for x in new_list:
		if x not in old_list:
			return_list.append(x)
	return return_list
class disk:
	devices = []
	path_to_device = {}
	path_to_disk = {}
	disks = []
	sd_disks = []
	size = []
	unit = []
	sector_size = []
	alphabet = ["a", "b", "c", "d", "e", "f", "g", "h",
 		"i", "j", "k", "l", "m", "n", "n", "o", "p", "q", "r", "s", "t",
 		"u", "v", "w", "x", "y", "z"]
	def __init__(self):
		#Clear disk values
		disk.disks = []
		disk.sd_disks = []
		disk.size = []
		disk.unit = []
		disk.path_to_disk = {}
		disk.path_to_device = {}
		sector_size = []
		
		#Probe for Devices
		disk.devices = parted.getAllDevices()
		
		# Add device paths
		for x in range(0, len(disk.devices)):
			disk.disks.append(disk.devices[x].path)
		
		#Dictionary, path: device
		for x in disk.disks:
			disk.path_to_device[x] = Device(x)
			disk.path_to_disk[x] = Disk(disk.path_to_device[x])
		
		#Add other disk information
		for x in disk.disks: disk.sd_disks.append(x.replace("/dev/", ""))
		for x in range(0, len(disk.disks)): disk.size.append(float(str(disk.devices[x].length).replace("MB", "")))
		for x in disk.disks: disk.unit.append("M")
def diskType(current_disk):
	return Disk(Device(current_disk)).type
if __name__ == '__main__':
	disk()
	part("/dev/sdb", "MiB")
	for x in range(0, len(part.paths)):
		print ("%s	%s	%s	%s	%s	%s	%s" % (part.paths[x], part.fileSystem[x], part.type[x], part.size[x], part.mounts[x], part.start[x], part.end[x]))
