#!/usr/bin/env python

"""

This script will generate a stage3

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


def script(host: Host, args=None):
	if host is None:
		raise RuntimeError("Host must be specified for stage3.py")
	
	# Make the host path and cd to it
	mkdir(host.get_path())
	cd(host.get_path())
	
	mirror = "http://mirrors.rit.edu/gentoo"
	url_pre = "%s/releases/%s/autobuilds" % (mirror, host.arch)
	
	metafile = "latest-stage3-%s.txt" % host.arch
	if args is not None:
		if "systemd" in args:
			metafile = "latest-stage3-%s-systemd.txt" % host.arch
	
	metaurl = "%s/%s" % (url_pre, metafile)
	if download(metaurl, ".stage3.latest") == 1:
		print("Failed to download %s" % metaurl)
		metafile = "latest-stage3-%s.txt" % host.arch
		metaurl = "%s/%s" % (url_pre, metafile)
		print("Trying %s" % metaurl)
		if download(metaurl, ".stage3.latest") == 1:
			print("Failed to download %s\nStop" % metaurl)
			raise RuntimeError("Failed to retrieve metadata")
	
	with open(".stage3.latest", "r") as metafp:
		lines = [x for x in metafp.readlines() if x[0] != "#"]
		
		url = "%s/%s" % (url_pre, lines[0].split(" ")[0])
		filename = url[url.rfind("/") + 1:]
		metafp.close()
	
	if download(url + ".DIGESTS", filename + ".DIGESTS") == 1:
		raise RuntimeError("Failed to retrieve DIGESTS file")
	
	digest_parse = digest(filename + ".DIGESTS")
	stfn = stat(filename)
	
	need_download = 1
	
	if stfn == 2:
		rm(filename)
	elif stfn == 1:
		test_hash = sha512_hash(filename)
		if test_hash != digest_parse[filename]:
			print("SHA512 mismatch %s != %s" % (test_hash, digest_parse[filename]))
			need_download = 1
		else:
			need_download = 0
	
	if need_download and download(url, filename) == 1:
		raise RuntimeError("Failed to retrieve stage3 from %s" % url)
	
	if extract(filename, ".") != 0:
		raise RuntimeError("Failed to extract stage3")
	
	print("Initializing autogentoo directories")
	mkdir("autogentoo")
	mkdir("autogentoo/log")
	mkdir("autogentoo/tmp")
	mkdir("autogentoo/pkg")
	
	print("Preparing /etc/portage/")
	rmdir("etc/portage/package.use")
	touch("etc/portage/package.use")
	touch("etc/portage/package.env")
	touch("etc/portage/package.accept_keywords")
	
	print("Settings make.profile")
	cd("etc/portage/")
	rm("make.profile")
	ln("/usr/portage/profiles/%s" % host.profile, "make.profile")
