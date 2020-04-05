#!/usr/bin/env python

"""

The emerge script is to be on of the simplest scripts
Output from this script must be parsable by autogentoo.org

Create a second log file for the emerge logs

"""

from script import *
from client import Host
import subprocess
import scripts.make_conf
import sys
import importlib


def script(_job_name: str, host: Host, args: str):
	importlib.reload(scripts.make_conf)
	scripts.make_conf.script(_job_name, host)
	
	print("[INFO] Starting Emerge for host %s" % host.id)
	print("[DEBUG] emerge --autounmask-continue --buildpkg --usepkg %s" % args)
	
	out = subprocess.run(["emerge", "--autounmask-continue", "--buildpkg", "--usepkg", *args.split(" ")], stdout=sys.stdout, stderr=sys.stderr)
	print("[INFO] Job finished")
	
	return out.returncode
