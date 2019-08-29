#!/usr/bin/env python

"""

This script will rewrite the portage config

The package_etc script will most likely be changed later in
favor of a more user friendly approach to editing portage configs

For now, autogentoo.org will send the entire config and package_etc will
write it to a file

"""

from script import *
from client import Host
from collections import namedtuple

Flag = namedtuple("Flag", "name status")

FLAG_DISABLE = 0
FLAG_ENABLE = 1
FLAG_UNSTABLE = 2
FLAG_UNSET = 3


class PortageEntry:
	def __init__(self, line, comments):
		self.line = line
		self.comments = comments
		
		splt = self.line.split(" ")
		
		self.atom = splt[0]
		self.flags = []
		for flag in splt[1:]:
			self.flags.append(PortageEntry.flag_parse(flag))
	
	@staticmethod
	def flag_parse(string: str):
		if string.startswith("-"):
			return Flag(string[1:], FLAG_DISABLE)
		elif string.startswith(""):
			return Flag(string[1:], FLAG_UNSTABLE)
		return Flag(string, FLAG_ENABLE)
	
	def set(self, flag_str):
		temp_flag = PortageEntry.flag_parse(flag_str)
		
		for f in self.flags:
			if f.name == flag_str:
				f.status = temp_flag
				return
		self.flags.append(temp_flag)
	
	def __str__(self):
		out = ""
		
		for comment in self.comments:
			out += comment
			out += "\n"
		
		out += self.atom + " "
		
		for flag in self.flags:
			if flag.status == FLAG_UNSTABLE:
				out += "~"
			elif flag.status == FLAG_DISABLE:
				out += "-"
			out += flag.name
			out += " "
		out += "\n\n"
		return out


class PortageFile:
	def __init__(self, filename):
		self.filename = filename
		self.fp = open(self.filename, "r")
		self.lines = self.fp.readlines()
		
		self.entries = []
		current_comments = []
		for line in self.lines:
			line = line.replace("\n", "")
			if not len(line):
				continue
			if line[0] == "#":
				current_comments.append(line)
				continue
			self.entries.append(PortageEntry(line, current_comments))
			current_comments = []
		
		self.fp.close()
	
	def get_entry(self, package_atom):
		for entry in self.entries:
			if entry.atom == package_atom:
				return entry
		
		return None
	
	def add_entry(self, entry: PortageEntry):
		self.entries.append(entry)
	
	def write(self):
		self.fp = open(self.filename, "w+")
		
		for entry in self.entries:
			self.fp.write(str(entry))
		
		self.fp.close()


def script(job_name: str, host: Host, file_name: str, content: str):
	cd(host.get_path())
	
	fn = "etc/portage/%s" % file_name
	
	print("Opening %s" % fn)
	pf = open(fn, "w+")
	
	print("Writing config: ")
	pf.write(content)
	sys.stdout.write(content)
	
	print("Closing file")
	
	pf.close()
