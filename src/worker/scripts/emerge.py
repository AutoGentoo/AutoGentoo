#!/usr/bin/env python

"""

The emerge script is to be on of the simplest scripts
Output from this script must be parsable by autogentoo.org

Create a second log file for the emerge logs

"""

from script import *
from client import Host
import subprocess
from .util import validate_chroot
import scripts.make_conf


def script(_job_name: str, host: Host, args: str):
	validate_chroot(_job_name, host)
	scripts.make_conf.script(_job_name, host)
	
	print("[INFO] Starting Emerge for host %s" % host.id)
	print("[DEBUG] emerge --autounmask-continue --buildpkg --usepkg %s" % " ".join(args))
	
	logfp = open("logs/%s.log" % _job_name, "a+")
	
	print("[DEBUG] chroot %s" % host.get_path())
	exit_code = chroot(host)
	if exit_code != 0:
		return exit_code
	
	subprocess.run(["emerge", "--autounmask-continue", "--buildpkg", "--usepkg", *args], stdout=logfp, stderr=logfp)
	
	print("[INFO] Job finished")
	
	return 0
