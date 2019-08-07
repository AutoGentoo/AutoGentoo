from os import system as s
import os
from shutil import *
import urllib.request
import urllib.error
import traceback

cp = copy2
mv = move
mkdir = os.makedirs
rm = os.remove
cd = os.chdir


def download(url, file_name):
	try:
		print("Downloading %s to %s..." % (url, file_name), end="")
		urllib.request.urlretrieve(url, file_name)
		print("done")
	except urllib.error.HTTPError:
		print("failed")
		traceback.print_exc()
		return 1
	return 0


def extract(filename: str, output_dir):
	if not os.path.exists(output_dir):
		mkdir(output_dir)
	
	print("Extracting %s to %s..." % (filename, output_dir), end="")
	if filename.endswith(".tar") or filename.endswith(".gz") or filename.endswith(".xz"):
		out = s("tar -xvf %s -C %s" % (filename, output_dir))
	elif filename.endswith(".zip"):
		out = s("unzip %s -d %s" % (filename, output_dir))
	elif filename.endswith(".bz2"):
		out = s("bzip2 -d %s" % filename)
	else:
		print("Invalid filetype %s" % filename[filename.rfind("."):])
		out = 1
	
	if out == 0:
		print("done")
	else:
		print("failed")
	
	return out


def chroot(host):
	cd(host.get_path())
	os.chroot(".")


def touch(filepath):
	open(filepath, "a+").close()
