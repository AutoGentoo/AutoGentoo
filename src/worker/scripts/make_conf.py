#!/usr/bin/env python

"""

This script will rewrite make.conf
take current variables from host and overwrite the file

"""

from client import Host
import pystache


def script(_job_name: str, host: Host, _args=None):
	filename = host.get_path("etc/portage/make.conf")
	
	fp = open(filename, "w+")
	template = open("templates/make.conf", "r").read()
	
	content = pystache.render(template, host)
	print(content)
	
	fp.write(content)
	fp.flush()
	fp.close()
