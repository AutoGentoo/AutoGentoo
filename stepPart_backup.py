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

import os
import subprocess
from gi.repository import Gtk, Gdk, GObject
from reparted import *
from reparted.device import probe_standard_devices

global global_unit
global_unit = "m"
class part:
	mounts = []
	format_types = []
	partnums = []
	partsizes = []
	partunits = []
	start = []
	start_unit = []
	end = []
	end_unit = []
	mounts_frees = []
	format_types_frees = []
	partnums_frees = []
	partsizes_frees = []
	partunits_frees = []
	start_frees = []
	start_unit_frees = []
	end_frees = []
	end_unit_frees = []
	primary = []
	parent = []
	extended = False
	partnum = 0
	startnum = 0
	def __init__(self, disk, unit):
		os.system("echo -n 'Clearing old values...'")
		part.mounts = []
		part.format_types = []
		part.partnums = [] # Number of Partition (Used for order)
		part.partsizes = []
		part.partunits = []
		part.start = []
		part.start_unit = []
		part.end = []
		part.end_unit = []
		part.primary = []
		part.parent = []
		extended = False
		part.partnum = 0
		os.system("echo 'Done'")
		os.system("partx -l %s > partnum" % (disk))
		partnum = open('partnum', "r").readlines()
		os.system("rm -rf partnum")
		partnum = int(len(partnum))
		#Extended: number of the extended partition, 0 if None
		extended = 0
		# Check if there is an extended partition
		if os.system("partx -o NR,TYPE %s | grep '0x5' > /dev/null" % disk) == 0:
			extended = subprocess.check_output("partx -r -o NR,TYPE %s | grep '0x5'" % disk, shell=True)
			extended = str(extended.decode("UTF-8"))
			extended = int(extended.replace(" 0x5", ""))
		if partnum == 1:
			print ("Found %d Partition" % partnum)
		else:
			print ("Found %d Partitions" % partnum)
		
		#Getting the Order of Partnums
		os.system("echo -n 'Finding Partitions...'")
		os.system("partprobe -s | grep %s > partnums" % disk)
		print ("done")
		partnum_file = open("partnums", "r").read()
		os.system("rm -rf partnums")
		partnum_file = partnum_file.replace("\n", "")
		partnum_file = partnum_file[partnum_file.index("partitions"):].replace("partitions ", "")
		extended_start = partnum_file.find("<") #Getting Start of the extended partition
		extended_end = partnum_file.find(">") #Getting end of the extended partition
		extended_val = partnum_file[extended_start+1:extended_end]
		extended_val = extended_val.split(" ")
		if extended_start != -1:
			extended = extended_start - 2
			extended = partnum_file[extended]
			part.extended = True
		else:
			extended = -1
			part.extended = False
		partnum_file = partnum_file.replace("<", "")
		partnum_file = partnum_file.replace(">", "")
		partnum_file = partnum_file.replace("partitions", "")
		part.partnums = partnum_file.split(" ")
		current_check = 0
		while current_check != len(part.partnums):
			if part.partnums[current_check] in extended_val:
				part.parent.append("%s%s" % (disk, extended))
				part.primary.append(0)
			else:
				part.parent.append(None)
				if part.partnums[current_check] != extended:
					part.primary.append(1)
				else:
					part.primary.append(0)
			current_check += 1
		current_num = 0
		if part.partnums[0] == "":
			part.partnums = []
		while current_num != len(part.partnums):
			os.system("echo -n 'Scanning Partition %s%s...'"  % (disk, part.partnums[current_num]))
			current_cmd = str(subprocess.check_output("partx -P -b -o SIZE,START,END %s%s" % (disk, part.partnums[current_num]), shell=True).decode("UTF-8"))
			value = []
			curr_char_num = 0
			curr_char = ""
			temp = ""
			#SIZE
			temp = get_value(current_cmd, curr_char_num)
			value.append(temp[0])
			#START
			temp = get_value(current_cmd, temp[1])
			value.append(temp[0])
			#END
			temp = get_value(current_cmd, temp[1], '\n')
			value.append(temp[0])
			#MOUNTPOINT and FSTYPE section
			check_exit = os.system("lsblk -P -o FSTYPE,MOUNTPOINT %s%s > /dev/null" % (disk, part.partnums[current_num]))
			if check_exit != 0:
				value.append('FSTYPE="extended"')
				value.append('MOUNTPOINT=""')
			else:
				current_cmd = str(subprocess.check_output("lsblk -P -o FSTYPE,MOUNTPOINT %s%s" % (disk, part.partnums[current_num]), shell=True).decode("UTF-8"))
				#FSTYPE
				temp = get_value(current_cmd, 0, ' ')
				value.append(temp[0])
				#MOUNPOINT
				temp = get_value(current_cmd, temp[1], '\n')
				value.append(temp[0])
			curr_exec = 0
			while True:
				try:
					exec("%s" % value[curr_exec])
				except:
					break
				curr_exec += 1
			print ("done")
			part.mounts.append(MOUNTPOINT)
			part.format_types.append(FSTYPE)
			part.partsizes.append(format_units_size(unit, SIZE))
			part.partunits.append(unit)
			part.start.append(format_units(unit, START))
			part.start_unit.append(unit)
			part.end.append(format_units(unit, END))
			part.end_unit.append(unit)
			current_num += 1
		part.mounts_frees = part.mounts
		part.format_types_frees = part.format_types
		part.partnums_frees = part.partnums
		part.partsizes_frees = part.partsizes
		part.partunits_frees = part.partunits
		part.start_frees = part.start
		part.start_unit_frees = part.start_unit
		part.end_frees = part.end
		part.end_unit_frees = part.end_unit
		
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
def format_units_size(unit, input_float, demical=2):
	unit = unit.lower()
	unit_list = ["k", "m", "g", "t"]
	unit_values = [1024, 1048576, 1073741824, 1099511627776]
	if unit in unit_list:
		listnum = unit_list.index(unit)
		temp_float = float(input_float)
		temp_float = temp_float/unit_values[listnum]
		input_float = str(round(temp_float, demical))
		return input_float
def format_units(unit, input_float, demical=2):
	unit = unit.lower()
	unit_list = ["k", "m", "g", "t"]
	unit_values = [1, 1024, 1048576, 1073741824]
	if unit in unit_list:
		listnum = unit_list.index(unit)
		temp_float = float(input_float)
		temp_float = temp_float/unit_values[listnum]
		input_float = str(round(temp_float, demical))
		return input_float
def get_iter(gtk_treestore, iters, string, column=0):
	if string == None:
		return None
	current_row = 0
	while gtk_treestore.get_value(iters[current_row], column) != string:
		current_row += 1
	return gtk_treestore.get_iter(gtk_treestore.get_path(iters[current_row]))
class disk:
	devices = []
	path_to_device = {}
	disks = []
	sd_disks = []
	size = []
	unit = []
	disk_name = ""
	def __init__(self, disk_num):
		#Clear disk values
		disk.disks = []
		disk.sd_disks = []
		disk.size = []
		disk.unit = []
		disk.disk_name = ""
		disk.path_to_device = {}
		
		#Probe for Devices
		disk.devices = probe_standard_devices()
		
		# Add device paths
		for x in range(0, len(disk.devices)):
			disks.disks.append(disk.devices[x].path)
		
		#Dictionary, path: device
		for x in disk.devices: disk.path_to_device[x] = Device(x)
		
		#Add other disk information
		for x in disk.disks: x.replace("/dev/", "")
		for x in disk.disks: disk.size.append(str(disk.devices[x].size).replace("MB", ""))
		for x in disk.disks: disk.unit.append("M")
		disk.disk_name = disk.devices[disk.disks[disk_num]].model
		
class find_free_space:
	free_parts = []
	free_start = []
	free_start_unit = []
	free_end = []
	free_end_unit = []
	free_size = []
	free_size_unit = []
	get_free_true = []
	def __init__(self, current_disk, current_unit):
		find_free_space.free_parts = []
		find_free_space.free_start = []
		find_free_space.free_start_unit = []
		find_free_space.free_end = []
		find_free_space.free_end_unit = []
		find_free_space.free_size = []
		find_free_space.free_size_unit = []
		find_free_space.get_free_true = []
		os.system("parted -s %s unit %siB print free > freespaceinfo_2.txt" % (current_disk, current_unit))
		os.system("cat freespaceinfo_2.txt | grep -i free > freespaceinfo.txt")
		find_part_free = open("freespaceinfo_2.txt", "r").readlines()
		find_part_free = find_part_free[7:]
		current_free_find_line = -1
		while current_free_find_line != len(find_part_free):
			current_free_find_line += 1
			try:
				current_line_free = find_part_free[current_free_find_line]
			except IndexError:
				break
			if current_line_free == "" or current_line_free == " " or current_line_free == "\n":
				break
			if current_line_free[0:3] != "   ":
				find_free_space.get_free_true.append(0)
			else:
				find_free_space.get_free_true.append(1)
		free_info_file = open("freespaceinfo.txt", "r").readlines()
		number_of_frees = len(free_info_file)
		current_num = -1
		while len(find_free_space.free_start) != number_of_frees:
			current_num += 1
			current_line = free_info_file[current_num]
			current_line = current_line.replace("\n", "")
			current_line = current_line.replace(" ", "")
			temp_array = []
			current_char = ""
			current_char_num = -1
			while current_char != "B":
				current_char_num += 1
				current_char = current_line[current_char_num]
				temp_array.append(current_char)
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp = str1 = ''.join(temp_array)
			find_free_space.free_start.append(float(temp))
			find_free_space.free_start_unit.append(str(current_line[current_char_num-2]))
			current_line = current_line[current_char_num+1:]
			temp_array = []
			current_char = ""
			current_char_num = -1
			while current_char != "B":
				current_char_num += 1
				current_char = current_line[current_char_num]
				temp_array.append(current_char)
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp = str1 = ''.join(temp_array)
			find_free_space.free_end.append(float(temp))
			find_free_space.free_end_unit.append(str(current_line[current_char_num-2]))
			current_line = current_line[current_char_num+1:]
			temp_array = []
			current_char = ""
			current_char_num = -1
			while current_char != "B":
				current_char_num += 1
				current_char = current_line[current_char_num]
				temp_array.append(current_char)
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp = str1 = ''.join(temp_array)
			find_free_space.free_size.append(float(temp))
			find_free_space.free_size_unit.append(str(current_line[current_char_num-2]))
def diskType(current_disk):
	try_parted = os.system("parted -s %s print >> diskinfo.txt" % (current_disk))
	if try_parted != 0:
		print ("Please run as root")
		exit(1)
	disktype = open("diskinfo.txt", "r").readlines()
	diskline = disktype[3]
	dtypelen = len(diskline)
	dtypelen = int(dtypelen)
	dtypeend = dtypelen - 1
	dtypeend = int(dtypeend)
	disk_type = diskline[17:dtypeend]
	os.system("rm -rf diskinfo.txt")
	return disk_type
