#!/usr/bin/env python

"""

This script will rewrite the portage config

The package_etc script will most likely be changed later in
favor of a more user friendly approach to editing portage configs

For now, autogentoo.org will send the entire config and package_etc will
write it to a file

"""

from script import *
from client import Host
import sys


def script(_job_name: str, host: Host, file_name: str, content: str):
	cd(host.get_path())
	
	fn = "etc/portage/%s" % file_name
	
	print("Opening %s" % fn)
	pf = open(fn, "w+")
	
	print("Writing config: ")
	pf.write(content)
	sys.stdout.write(content)
	
	print("Closing file")
	
	pf.close()
