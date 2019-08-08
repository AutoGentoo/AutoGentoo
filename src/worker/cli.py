#! /usr/bin/env python

from client import Server
from autogentoo_api.request import *


class Cli:
	def __init__(self, path):
		self.path = path
		self.server = Server(self.path)

	def cli(self):
		while True:
			self.server.read()
			line = input("autogentoo > ")
			splt = line.split(" ")
			
			print(eval("self.%s(*%s)" % (splt[0], splt[1:])))
	
	def send_request(self, request_str, args):
		client = Client(Address("localhost", 9491))
		stat = "OK"
		
		try:
			response = client.request(request_str, args)
		except ConnectionError:
			stat = "CON"
			response = None
		else:
			if response.code == 200:
				stat = "OK"
			elif response.code != 403:
				if response.code < 500:
					stat = "CLT"
				else:
					stat = "ERR"
		return response.code, response.content, stat
	
	def stage3(self, host_id, args=""):
		code, content, status = self.send_request("HOST_STAGE3", (
			authorize("cli", self.server.sudo_token),
			host_select(host_id),
			job_select(args)
		))
		
		print(code)
		print(status)
		print(content)


def main(args):
	cli = Cli(args[1])
	cli.cli()


if __name__ == "__main__":
	import sys
	main(sys.argv)
