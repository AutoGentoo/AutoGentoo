#!/usr/bin/env python

"""

This script will rewrite make.conf
take current variables from host and overwrite the file

"""

from client import Host
from string import Template


def script(_job_name: str, host: Host, _args=None, root="/"):
	filename = root + "etc/portage/make.conf"
	
	fp = open(filename, "w+")
	template = open(root + "templates/make.conf", "r").read()
	
	t = Template(template)
	fp.write(t.substitute(host))
	
	fp.write("\n")
	for key in host.extra:
		if key == "video":
			continue
		fp.write("%s=\"%s\"\n" % (key, host.extra[key]))
	
	fp.flush()
	fp.close()
