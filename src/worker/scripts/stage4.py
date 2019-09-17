#!/usr/bin/env python

"""

This script will regenerate installed packages with correct CFLAGS
Will run an emerge world with --emptytree

Steps:
	- Figure out which file to download based on profile and arch
	- Download from http://distfiles.gentoo.org
	- Extract this file
	- Create /autogentoo/{log,pkg,tmp}
	- Generate package.use, package.env, package.accept_keywords so they don't become directories
	- Regenerate make.conf

"""

from script import *
from client import Host

def script(_job_name: str, host: Host, args=None):
	
	
	pass