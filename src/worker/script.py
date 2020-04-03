import os
from shutil import *
import urllib.request
import urllib.error
import traceback
import functools
import hashlib
import subprocess

print = functools.partial(print, flush=True)
cp = copy2
mv = move
rm = os.remove
rmdir = os.rmdir
pwd = os.getcwd()
logfp = None


def s(cmd):
	out = subprocess.run(cmd, shell=True, stdout=logfp, stderr=logfp)
	return out.returncode


def cd(path: str):
	os.chdir(path)


def mkdir(path):
	return os.makedirs(path, exist_ok=True)


def download(url, file_name):
	try:
		print("Downloading %s to %s..." % (url, file_name), end="")
		urllib.request.urlretrieve(url, file_name + ".temp")
		mv(file_name + ".temp", file_name)
		print("done")
	except urllib.error.HTTPError:
		print("failed")
		traceback.print_exc()
		return 1
	return 0


def extract(filename: str, output_dir):
	if os.getuid() != 0:
		raise PermissionError("Cannot create /dev if not root!")
	
	if not os.path.exists(output_dir):
		mkdir(output_dir)
	
	print("Extracting %s to %s ..." % (filename, output_dir), end="")
	if filename.endswith(".tar") or filename.endswith(".gz") or filename.endswith(".xz") or filename.endswith(".bz2"):
		out = s("tar -xf %s -C %s" % (filename, output_dir))
	elif filename.endswith(".zip"):
		out = s("unzip %s -d %s" % (filename, output_dir))
	else:
		print("Invalid filetype %s" % filename[filename.rfind("."):])
		out = 1
	
	if out == 0:
		print("done")
	else:
		print("failed")
		print("Error [%d] %s" % (out, os.strerror(out)))
	
	return out


def touch(filepath):
	open(filepath, "a+").close()


def digest(file):
	fp = open(file, "r")
	
	out = {}
	
	current_type = ""
	for line in fp.readlines():
		# "# SHA512 HASH"
		line = line.replace("\n", "")
		
		if line.startswith("# "):
			current_type = line[2:line.find(" HASH")].lower()
			continue
		
		if current_type != "sha512":
			continue
		
		hashstr, filename = line.split("  ")
		out[filename] = hashstr
	
	fp.close()
	return out


def sha512_hash(filename):
	chunksize = 65536
	sha_hash = hashlib.sha512()
	
	with open(filename, "rb") as f:
		data = f.read(chunksize)
		while data:
			sha_hash.update(data)
			data = f.read(chunksize)
	
	return sha_hash.hexdigest()


def stat(path):
	if not os.path.exists(path):
		return 0
	return 1 if os.path.isfile(path) else 2


def ln(file, link_name):
	s("ln -s %s %s" % (file, link_name))


def rmrf(path):
	s("rm -rf %s" % path)
