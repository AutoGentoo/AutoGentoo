#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  install.py
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


import os, sys, subprocess

class color:
	ESC_SEQ="\x1b["
	END=("%s39;49;00m" % (ESC_SEQ))
	RED=("%s31;01m" % (ESC_SEQ))
	GREEN=("%s32;01m" % (ESC_SEQ))
	YELLOW=("%s33;01m" % (ESC_SEQ))
	BLUE=("%s34;01m" % (ESC_SEQ))
	MAGENTA=("%s35;01m" % (ESC_SEQ))
	CYAN=("%s36;01m" % (ESC_SEQ))
	BOLD=("\033[1m")

def main():
	if os.getenv('USERNAME') != "root":
		print ("%s%sError: %s You must run as the root user!" % (color.RED, color.BOLD, color.END))
		exit(1)
	print ("Installing Autogentoo...")
	print ("Creating directory /usr/lib/autogentoo")
	if os.path.exists("/usr/lib/autogentoo"):
		os.system("rm -rf /usr/lib/autogentoo/")
	os.system("mkdir /usr/lib/autogentoo")
	print ("Entering directory %s" % os.path.abspath(os.path.dirname(__file__)))
	os.chdir(os.path.abspath(os.path.dirname(__file__)))
	print ("Copying Contents from root directory")
	os.system("cp -R * /usr/lib/autogentoo")
	print ("Creating %sautogentoo%s command" % (color.BOLD, color.END))
	if os.path.exists("/usr/bin/autogentoo"):
		os.system("rm -rf /usr/bin/autogentoo")
	os.system("cp -R AutoGentoo /usr/bin/autogentoo")
	os.system("chmod 744 /usr/bin/autogentoo")
	if os.path.exists("/usr/share/applications/AutoGentoo.desktop"):
		os.system("rm -rf /usr/share/applications/AutoGentoo.desktop")
	os.system("cp AutoGentoo.desktop /usr/share/applications/")
	print ("Done")
	return 0

if __name__ == '__main__':
	main()

