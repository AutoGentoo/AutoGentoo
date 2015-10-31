#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  partitions.py
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

from reparted import *
from reparted.device import probe_standard_devices
import sys
class devices:
	def __init__(self, print_device=None, disk_property="model"):
		paths = []
		self.disks = {}
		disk = []
		y = probe_standard_devices() # Defined such that 'probe_standard_devices()' isn't initialized twice
		for x in range(0, len(y)):
			paths.append(y[x].path)
		for x in paths: 
			self.disks[x] = Device(x)
			disk.append(Disk(self.disks[x]))
		for x in range(0, len(disk[0].partitions())): print disk[0].partitions()[x].geom
def main(argv):
	try:
		devices(argv[1], argv[2])
	except IndexError:
		try:
			devices(argv[1])
		except IndexError:
			devices()
	return 0

if __name__ == '__main__':
	main(sys.argv)

