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
from client import Host, Server
from scripts import make_conf


def script(_job_name: str, host: Host, args=None, bootstrap=False):
	if host is None:
		raise RuntimeError("Host must be specified for stage3.py")
	
	# Make the host path and cd to it
	ROOT = ""
	if bootstrap:
		ROOT = os.path.abspath(host.get_path())
		if stat(ROOT + "/templates") == 2:
			rmrf(ROOT + "/templates")
		copytree(os.path.dirname(__file__) + "/templates", ROOT + "/templates")
	
	stage_dist = ROOT + "/autogentoo/stage_dist"
	
	print("Initializing autogentoo directories")
	mkdir(ROOT + "/autogentoo")
	mkdir(stage_dist)
	mkdir(ROOT + host.portage_logdir)
	mkdir(ROOT + host.portage_tmpdir)
	mkdir(ROOT + host.pkgdir)
	mkdir(ROOT + host.portdir)
	
	mirror = "http://mirrors.rit.edu/gentoo"
	url_pre = "%s/releases/%s/autobuilds" % (mirror, host.arch)
	
	metafile = ROOT + "/autogentoo/stage_dist/latest-stage3-%s.txt" % host.arch
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
		filename = url[url.rfind("/") + 1:]
		metafp.close()
	
	if download(url + ".DIGESTS", filename + ".DIGESTS") == 1:
		raise RuntimeError("Failed to retrieve DIGESTS file")
	
	digest_parse = digest(stage_dist + "/" + filename + ".DIGESTS")
	stfn = stat(stage_dist + "/" + filename)
	
	need_download = 1
	
	if stfn == 2:
		rm(stage_dist + "/" + filename)
	elif stfn == 1:
		test_hash = sha512_hash(stage_dist + "/" + filename)
		if test_hash != digest_parse[filename]:
			print("SHA512 mismatch %s != %s" % (test_hash, digest_parse[filename]))
			need_download = 1
		else:
			need_download = 0
	
	if "nodownload" not in args and need_download and download(url, stage_dist + "/" + filename) == 1:
		raise RuntimeError("Failed to retrieve stage3 from %s" % url)
	
	if "noextract" not in args and extract(stage_dist + "/" + filename, ROOT + "/") != 0:
			raise RuntimeError("Failed to extract stage3")
	
	print("Preparing /etc/portage/")
	mkdir(ROOT + "/etc/portage/")
	rmrf(ROOT + "/etc/portage/package.use")
	touch(ROOT + "/etc/portage/package.use")
	touch(ROOT + "/etc/portage/package.env")
	touch(ROOT + "/etc/portage/package.accept_keywords")
	
	print("Settings make.profile")
	cd(ROOT + "/etc/portage/")
	
	try:
		rm("make.profile")
	except FileNotFoundError:
		pass
	
	ln("%s/profiles/%s" % (host.portdir, host.profile), "make.profile")
	
	cd(ROOT + "/")
	
	print("Updating /etc/portage/make.conf")
	make_conf.script(_job_name, host)
	
	touch(".stage3")
	
	print("stage3 successful")


def main(argv):
	"""
	Create a bootstrap for the stage3 script
	so that it can run outside the chroot
	
	:param argv: args passed in from command line
	:return: exit status
	"""
	
	parent_dir = argv[1]
	host_id = argv[2]
	
	args = ()
	if len(argv) == 4:
		args = argv[3].split(",")
	
	parent = Server(parent_dir)
	parent.read()
	
	host = parent.hosts[host_id]
	job_name = "%s_stage3" % host_id
	
	mkdir("%s/logs" % parent_dir)
	
	global logfp
	logfp = open("%s/logs/%s.log" % (parent_dir, job_name), "a+")
	
	sys.stdout = logfp
	sys.stderr = logfp
	
	print("")
	print("------ new stage3 ------")
	script(job_name, host, args, bootstrap=True)
	
	return 0


if __name__ == "__main__":
	import sys
	exit(main(sys.argv))
