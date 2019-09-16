#!/usr/bin/env python

"""

The emerge script is to be on of the simplest scripts
Output from this script must be parsable by autogentoo.org

Create a second log file for the emerge logs

"""

from script import *
from client import Host
import subprocess


def script(job_name: str, host: Host, args: str):
	print("[INFO] Starting Emerge for host %s" % host.id)
	print("[DEBUG] chroot %s" % host.get_path())
	chroot(host)
	
	print("[DEBUG] emerge --autounmask-continue --buildpkg --usepkg %s" % args)
	lfile = "emerge_logs/%s-%s.log" % (host.id, job_name)
	
	print("[INFO] Emerge log in %s" % lfile)
	
	global logfp
	logfp = open(lfile, "w+")
	subprocess.run(["emerge", "--autounmask-continue", "--buildpkg", "--usepkg", *args], stdout=logfp, stderr=logfp)
	
	print("[INFO] Job finished")
	
	return 0
