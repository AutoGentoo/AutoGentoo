#! /usr/bin/env python

from client import Server
from autogentoo_api.request import *
import pprint
import atexit
import rlcompleter
import readline
import traceback
import time

histfile = ".autogentoo_history"
try:
	readline.read_history_file(histfile)
	# default history len is -1 (infinite), which may grow unruly
	readline.set_history_length(1000)
except FileNotFoundError:
	pass

atexit.register(readline.write_history_file, histfile)


class Cli:
	def __init__(self, path):
		self.path = path
		self.server = Server(self.path)
		self.commands = {
			"stage3": ("host_id", "args"),
			"mkhost": ("arch", "profile", "hostname"),
			"ls": (),
			"help": (),
			"q": (),
		}
		
		self.completion = self.commands
		
		readline.set_completer(rlcompleter.Completer(self.completion).complete)
		readline.parse_and_bind("tab: complete")
		
		self.pp = pprint.PrettyPrinter(indent=1)

	def update_completion(self):
		for host in self.server.hosts:
			self.completion[host.id] = host
		
		readline.set_completer(rlcompleter.Completer(self.completion).complete)

	def cli(self):
		time.sleep(0.1)
		while True:
			self.server.read()
			self.update_completion()
			try:
				line = input("autogentoo > ").replace("\n", "").strip()
				if not len(line):
					continue
			except KeyboardInterrupt:
				break
			
			try:
				res = self.command(line)
			except KeyboardInterrupt:
				print("")
				continue
			except Exception:
				traceback.print_exc(file=sys.stdout)
			else:
				if res is not None:
					print(res)
	
	def command(self, cmd_name):
		try:
			arg_enum = self.commands[cmd_name]
		except KeyError:
			return "Command %s not found" % cmd_name
		
		args = []
		for i in arg_enum:
			args.append(input("%s: " % i).replace("\n", ""))
		
		return eval("self.%s(*args)" % cmd_name)
	
	@staticmethod
	def send_request(request_str, args):
		client = Client(Address("localhost", 9491))
		stat = "OK"
		
		try:
			response = client.request(request_str, args)
		except ConnectionError:
			stat = "CON"
			response = None
			
			return None, None, stat
		else:
			if response.code == 200:
				stat = "OK"
			elif response.code != 403:
				if response.code < 500:
					stat = "CLT"
				else:
					stat = "ERR"
			
			time.sleep(0.5)
			return response.code, response.content, stat
	
	def stage3(self, host_id, args=""):
		code, content, status = self.send_request("REQ_HOST_STAGE3", [
			authorize("cli", self.server.sudo_token),
			host_select(host_id),
			job_select(args)
		])
		
		print(content)
	
	def mkhost(self, arch, profile, hostname):
		code, content, status = self.send_request("REQ_HOST_NEW", [
			authorize("cli", self.server.sudo_token),
			host_new(arch, profile, hostname)
		])
	
	def ls(self):
		for host in self.server.hosts:
			print("id: %s" % host.id)
			print("path: %s" % host.get_path())
			print("arch %s" % host.arch)
			print("profile %s" % host.profile)
			print("")
		
		if not len(self.server.hosts):
			print("No hosts")
	
	def help(self):
		self.pp.pprint(self.commands)
	
	def q(self):
		exit(0)


def main(args):
	cli = Cli(args[1])
	cli.cli()


if __name__ == "__main__":
	import sys
	main(sys.argv)
