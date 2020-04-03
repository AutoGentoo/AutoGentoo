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


def script(_job_name: str, host: Host, args: str):
	scripts.make_conf.script(_job_name, host)
	
	print("[INFO] Starting Emerge for host %s" % host.id)
	print("[DEBUG] emerge --autounmask-continue --buildpkg --usepkg %s" % " ".join(args))
	
	subprocess.run(["emerge", "--autounmask-continue", "--buildpkg", "--usepkg", *args], stdout=logfp, stderr=logfp)
	print("[INFO] Job finished")
	
	return 0
