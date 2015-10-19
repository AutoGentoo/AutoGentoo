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
	partnum = 0
	def __init__(self, disk, unit):
		os.system("echo -n 'Clearing old values...'")
		part.mounts = []
		part.format_types = []
		part.partnums = []
		part.partsizes = []
		part.partunits = []
		part.start = []
		part.start_unit = []
		part.end = []
		part.end_unit = []
		part.primary = []
		part.partnum = 0
		os.system("echo 'Done'")
		os.system("parted -s %s unit %siB print > partinfo.txt" % (disk, unit))
		partnum = sum(1 for line in open('partinfo.txt', "r").readlines())
		partnum -= 8
		check_extended = os.system("cat partinfo.txt | grep extended > /dev/null")
		os.system("rm -rf partinfo.txt")
		os.system("lsblk -o KNAME,SIZE,TYPE | grep part | grep %s > partinfo.txt" % disk[5:8])
		try_unit = os.system("parted -s /dev/%s unit %siB print > partinfo_main.txt" % (disk[5:8], unit))
		if try_unit != 0:
			print ("Unit %siB not found by parted!" % global_unit)
			exit (1)
		find_part_info = open("partinfo_main.txt", "r").readlines()
		find_part_info = find_part_info[7:]
		if partnum == 1:
			print ("Found %d Partition" % partnum)
		else:
			print ("Found %d Partitions" % partnum)
		current_num = -1
		while len(part.partnums) != partnum:
			current_num += 1
			#Editing the parted partition line
			current_part = find_part_info[current_num]
			#Partition Numbers
			temp_num_str = str(current_num)
			current_part_num = current_part[0:5]
			current_part_num = current_part_num.decode("UTF-8")
			current_part_num = current_part_num.replace(" ", "")
			current_part_num = int(current_part_num)
			part.partnums.append(current_part_num)
			print ("Scanning partition %s%s" % (disk, part.partnums[current_num]))
			#Removing Spaces and newlines
			current_part = current_part.decode("UTF-8")
			current_part = current_part.replace("\n", "")
			current_part = current_part.replace(" ", "")
			#Remove partition number from string
			current_part = current_part[1:]
			find_primary = os.system("echo '%s' | grep primary > /dev/null" % current_part)
			if find_primary != 0:
				part.primary.append(0)
			else:
				part.primary.append(1)
			#Mount Points
			try:
				find_mount = subprocess.check_output("lsblk %s%s -o MOUNTPOINT | grep -v MOUNTPOINT" % (disk, part.partnums[current_num]), shell=True)
			except subprocess.CalledProcessError:
				find_mount = " "
			else:
				find_mount = subprocess.check_output("lsblk %s%s -o MOUNTPOINT | grep -v MOUNTPOINT" % (disk, part.partnums[current_num]), shell=True)
			find_mount = find_mount.decode("UTF-8")
			find_mount = find_mount.replace("\n", "")
			find_mount = str(find_mount.replace(" ", ""))
			#Changing swap mount display
			if find_mount == "[SWAP]":
				find_mount = "swap"
			part.mounts.append(find_mount)
			#Getting essential Partition Information
			current_char_num = -1
			current_char = ""
			temp_array =[]
			#Part START
			while current_char != "B":
				current_char_num += 1
				current_char = current_part[current_char_num]
				temp_array.append(current_char)
			#Removing the unit from the float array
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			#Creating a string from array
			temp = str1 = ''.join(temp_array)
			#Adding Values to array
			part.start.append(float(temp))
			part.start_unit.append(str(current_part[current_char_num-2]))
			current_part = current_part[current_char_num+1:]
			#Reset the variables
			current_char_num = -1
			current_char = ""
			temp_array = []
			#Part END
			while current_char != "B":
				current_char_num += 1
				current_char = current_part[current_char_num]
				temp_array.append(current_char)
			#Removing the unit from the float array
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			#Creating a string from array
			temp = str1 = ''.join(temp_array)
			#Adding Values to array
			part.end.append(float(temp))
			part.end_unit.append(str(current_part[current_char_num-2]))
			current_part = current_part[current_char_num+1:]
			#Reset the variables
			current_char_num = -1
			current_char = ""
			temp_array = []
			while current_char != "B":
				current_char_num += 1
				current_char = current_part[current_char_num]
				temp_array.append(current_char)
			#Removing the unit from the float array
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			temp_array.remove(temp_array[len(temp_array)-1])
			#Creating a string from array
			temp = str1 = ''.join(temp_array)
			#Adding Values to array
			part.partsizes.append(float(temp))
			part.partunits.append(str(current_part[current_char_num-2]))
			current_part = current_part[current_char_num+1:]
			try:
				current_fs_type = str(subprocess.check_output("lsblk %s%s -o FSTYPE | grep -v FSTYPE" % (disk, part.partnums[current_num]), shell=True))
			except subprocess.CalledProcessError:
				current_fs_type = "extended"
			else:
				current_fs_type = str(subprocess.check_output("lsblk %s%s -o FSTYPE | grep -v FSTYPE" % (disk, part.partnums[current_num]), shell=True))
			if current_fs_type == "\n":
				current_fs_type = "extended"
			partition_type = current_fs_type.replace("\n", "")
			part.format_types.append(str(partition_type))
		os.system("rm -rf partinfomaininfo.txt")
		os.system("rm -rf mountinfo.txt")
		os.system("rm -rf partinfo_main.txt")
		os.system("rm -rf partinfo.txt")
		part.mounts_frees = part.mounts
		part.format_types_frees = part.format_types
		part.partnums_frees = part.partnums
		part.partsizes_frees = part.partsizes
		part.partunits_frees = part.partunits
		part.start_frees = part.start
		part.start_unit_frees = part.start_unit
		part.end_frees = part.end
		part.end_unit_frees = part.end_unit
class disk:
	alphabet = ["a", "b", "c", "d", "e", "f", "g", "h",
		"i", "j", "k", "l", "m", "n", "n", "o", "p", "q", "r", "s", "t",
		"u", "v", "w", "x", "y", "z"]
	disks = []
	sd_disks = []
	size = []
	unit = []
	main_disk_from_disk = 0
	disk_name = ""
	def __init__(self, disk_num):
		disk.disks = []
		disk.sd_disks = []
		disk.size = []
		disk.unit = []
		disk.main_disk_from_disk = 0
		disk.disk_name = ""
		os.system('lsblk -o KNAME,TYPE,SIZE,MODEL | grep -i """disk""" > disks.txt')
		disknum = sum(1 for line in open('disks.txt'))
		os.system("rm -rf disks.txt")
		print "Disknum", disknum
		number = []
		number.append(disknum)
		placeholder = disknum
		while len(number) != disknum:
			placeholder -= 1
			number.append(placeholder)
		print ("Number", number)
		current_num = 0
		while len(disk.disks) != disknum:
			current_num += 1
			current_disk = number[-current_num]
			disk.sd_disks.append("sd%s" % disk.alphabet[current_disk-1])
			disk.disks.append("/dev/sd%s" % disk.alphabet[current_disk-1])
			if len(disk.disks) == disknum:
				print ("Breaking")
				break
		current_num = 0
		while len(disk.size) != disknum:
			current_num += 1
			current_disk = number[-current_num]
			try:
				find_info = subprocess.check_output("""lsblk -o KNAME,SIZE | grep '%s '""" % (disk.sd_disks[current_num-1]), shell=True)
			except subprocess.CalledProcessError:
				disk.unit.append(global_unit)
				disk.size.append(0)
			else:
				find_info = find_info.decode("UTF-8")
				find_info = find_info.replace("\n", "")
				find_info = find_info[4:]
				find_info = find_info.replace(" ", "")
				find_unit = str(find_info[-1])
				disk.unit.append(find_unit)
				current_unit = str(disk.unit[current_num-1])
				size_find = float(find_info.replace(current_unit, ""))
				disk.size.append(size_find)
		try:
			disk.disk_name = ("%s   (%s%siB)" % (disk.disks[disk_num], disk.size[disk_num], disk.unit[disk_num]))
			disk.main_disk_from_disk = disk.disks[disk_num]
		except IndexError:
			print ("""Disk "/dev/sd%s" is not found""" % disk.alphabet[disk_num])
			exit(1)
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
		os.system("parted -s %s unit %siB print free | grep -i free > freespaceinfo.txt" % (current_disk, current_unit))
		os.system("parted -s %s unit %siB print free > freespaceinfo_2.txt" % (current_disk, current_unit))
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
