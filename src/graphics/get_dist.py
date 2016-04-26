#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  gpu_test.py
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


import subprocess, os, platform
from HTMLParser import HTMLParser

os.chdir ( os.path.dirname ( os.path.realpath ( __file__ ) ) )

class hrefParser(HTMLParser):
	def handle_starttag(self, tag, attrs):
		for attr in attrs:
			if attr[0] == "href":
				self.link = attr[1]
	def handle_data(self, data):
		if not data == '\r\n':
			self.name = data
class stringParser(HTMLParser):
	data = []
	def handle_data(self, data):
		self.data.append(data)
class gpu:
	vendor = ""
	product_full = ""
	product_id = ""
	product = ""
	vendor_class = ""
	driver_version = ""
	working_driver_version = ""
	version_list = []
	echo_list = []
	dist_location = ""
	def __init__(self):
		if ( os.system ( "ls graphics/nvidia > /dev/null 2>&1" ) != 0 ):
			os.system ( "mkdir -p graphics/nvidia" )
			os.system ( "mkdir -p graphics/radeon" )
		gpu.vendor = ""
		gpu.product_full = ""
		gpu.product_id = ""
		gpu.product = ""
		gpu.vendor = str(subprocess.check_output("lshw -class display | grep vendor", shell=True))
		gpu.vendor = gpu.vendor[15:]
		gpu.vendor = gpu.vendor.replace("\n", "")
		gpu.product_full = str(subprocess.check_output("lshw -class display | grep product", shell=True))
		gpu.product_full = gpu.product_full[16:]
		gpu.product_id_temp = []
		current_char = ""
		current_char_num = -1
		while current_char != "[":
			current_char_num += 1
			current_char = gpu.product_full[current_char_num]
			gpu.product_id_temp.append(current_char)
		gpu.product_id = str1 = str(''.join(gpu.product_id_temp))
		gpu.product_id = gpu.product_id.replace(" ", "")
		gpu.product_id = gpu.product_id.replace("[", "")
		gpu.product = gpu.product_full.replace("%s" % gpu.product_id, "")
		gpu.product = gpu.product.replace("[", "")
		gpu.product = gpu.product.replace("]", "")
		gpu.product = gpu.product.replace("\n", "")
		gpu.product = gpu.product.replace(" ", "", 1)
		if gpu.vendor.find("NVIDIA") != -1:
			gpu.vendor_class = "nvidia"
		else:
			gpu.vendor_class = "radeon"
		if gpu.vendor_class == "nvidia":
			get_version = get_nvidia_versions()
			gpu.version_list = get_version[0]
			os.system("rm -rf ./graphics/nvidia/nvidia*")
			get_nvidia_supported()
			get_echo()
			version_list = gpu.version_list
			find_supported = 1 # Placeholder
			current_version = -1
			while find_supported != 0:
				current_version += 1
				find_supported = os.system("cat ./graphics/nvidia/nvidia-%s.supported_cards | grep -i '%s' > /dev/null" % (version_list[current_version], gpu.product))
			gpu.driver_version = version_list[current_version]
			gpu.working_driver_version = gpu.driver_version
			gpu.dist_location = "http://us.download.nvidia.com/XFree86/Linux-{1}/{0}/NVIDIA-Linux-{1}-{0}.run".format(gpu.driver_version, platform.machine())
		elif gpu.vendor_class == "radeon":
			gpu.driver_version = "x11-drivers/radeon-ucode"
			gpu.working_driver_version = gpu.driver_version
		os.system ( "rm -rf graphics" )
def get_nvidia_versions():
	os.system("wget -q 'http://www.nvidia.com/object/unix.html'")
	name_list = []
	href_list = []
	return_list = []
	unix_file = open("unix.html", "r").readlines()
	for x in unix_file:
		if x.find("/AMD64/") != -1:
			amd_line = x
			break
	x = unix_file.index(amd_line)
	parser = hrefParser()
	for line in range(x, x+7):
		parser.feed(unix_file[line+1])
		if parser.link[0] == "/":
			parser.link = "http://www.nvidia.com%s" % parser.link
		name_list.append(parser.name)
		href_list.append(parser.link)
	return_list.append(name_list)
	return_list.append(href_list)
	os.system("rm -rf unix.html") 
	return return_list
def get_nvidia_supported():
	returned = get_nvidia_versions()
	link_list = returned[1]
	name_list = returned[0]
	for x in link_list:
		os.system("wget -q -O curr_file.html %s" % x)
		curr_file = open("curr_file.html", "r").readlines()
		os.system("rm -rf curr_file.html")
		for y in curr_file:
			if y.find("tab2_content") != -1:
				tab_line = curr_file.index(y)
				break
		data_line = curr_file[tab_line+1]
		classdata = stringParser()
		classdata.feed(data_line)
		data = '\n'.join(classdata.data)
		link_num = link_list.index(x)
		file_name = "nvidia-%s.supported_cards" % name_list[link_num]
		_file = open("graphics/nvidia/%s" % file_name, "w+")
		_file.write(data)
		_file.close()
def get_echo():
	for x in gpu.version_list:
		for y in x:
			if y == ".":
				num = x.index(y)
				gpu.echo_list.append(x[:num])
				break

def main():
	gpu()
	mask = open ( "/etc/portage/package.mask/nvidia-drivers", "w+" )
	mask.write ( ">=x11-drivers/nvidia-drivers-%s\n" % gpu.driver_version );
	print ( gpu.dist_location )
	return 0

if __name__ == '__main__':
	main()

