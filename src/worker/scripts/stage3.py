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
from . import make_conf


def script(_job_name: str, host: Host, args=None):
	if host is None:
		raise RuntimeError("Host must be specified for stage3.py")
	
	# Make the host path and cd to it
	cd("")
	
	stage_dist = "/autogentoo/stage_dist"
	
	print("Initializing autogentoo directories")
	mkdir("/autogentoo")
	mkdir(stage_dist)
	mkdir(host.portage_logdir)
	mkdir(host.portage_tmpdir)
	mkdir(host.pkgdir)
	mkdir(host.portdir)
	
	mirror = "http://mirrors.rit.edu/gentoo"
	url_pre = "%s/releases/%s/autobuilds" % (mirror, host.arch)
	
	metafile = "/autogentoo/stage_dist/latest-stage3-%s.txt" % host.arch
	if args is not None:
		if "systemd" in args:
			metafile = "latest-stage3-%s-systemd.txt" % host.arch
	
	metaurl = "%s/%s" % (url_pre, metafile)
	metatarget = stage_dist + "/.stage3.latest"
	if download(metaurl, metatarget) == 1:
		print("Failed to download %s" % metaurl)
		metafile = "latest-stage3-%s.txt" % host.arch
		metaurl = "%s/%s" % (url_pre, metafile)
		print("Trying %s" % metaurl)
		if download(metaurl, metatarget) == 1:
			print("Failed to download %s\nStop" % metaurl)
			raise RuntimeError("Failed to retrieve metadata")
	
	with open(metatarget, "r") as metafp:
		lines = [x for x in metafp.readlines() if x[0] != "#"]
		
		url = "%s/%s" % (url_pre, lines[0].split(" ")[0])
		filename = stage_dist + "/" + url[url.rfind("/") + 1:]
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
	
	print("Preparing /etc/portage/")
	mkdir("/etc/portage/")
	rmrf("/etc/portage/package.use")
	touch("/etc/portage/package.use")
	touch("/etc/portage/package.env")
	touch("/etc/portage/package.accept_keywords")
	
	print("Settings make.profile")
	cd("/etc/portage/")
	
	try:
		rm("make.profile")
	except FileNotFoundError:
		pass
	
	ln("%s/profiles/%s" % (host.portdir, host.profile), "make.profile")
	
	cd("/")
	
	print("Updating /etc/portage/make.conf")
	make_conf.script(_job_name, host)
	
	touch(".stage3")
