#! /usr/bin/env python
import string
import sys
import threading
import importlib
import signal
import socket
from random import random
from typing import Union

from util import SockStream
from .script import *
from client import Host


class WorkerThread(threading.Thread, SockStream):
	logfile: str
	job_name: str
	working_dir: str
	pid: int
	status: int
	
	host: Union[Host, None]
	command: str
	args: list
	res: int
	
	def __init__(self, job_name: str, client: socket.socket):
		threading.Thread.__init__(self)
		SockStream.__init__(self, client)
		
		self.job_name = job_name
		self.working_dir = os.getcwd()
		self.module = None
		self.status = -1
		self.res = -1
		
		self.args = []
	
	def lock(self):
		cwd = pwd()
		cd(self.working_dir)
		touch(self.logfile + ".lck")
		cd(cwd)
	
	def unlock(self):
		cwd = pwd()
		cd(self.working_dir)
		rm(self.logfile + ".lck")
		cd(cwd)
	
	def run_job(self):
		self.pid = os.fork()
		if self.pid == -1:
			return -1
		elif self.pid == 0:
			# Run the script
			importlib.reload(self.module)
			
			logfp = open(self.logfile, "a+")
			sys.stdout = logfp
			sys.stderr = logfp
			
			try:
				ret = self.module.script(self.job_name, self.host, self.args)
			except Exception:
				traceback.print_exc()
				ret = 1
			
			logfp.close()
			exit(ret)
		else:
			pid, self.status = os.waitpid(self.pid, 0)
			return self.status
	
	def kill(self):
		print("Killing %s (%s)" % (self.job_name, self.pid))
		os.kill(self.pid, signal.SIGKILL)
	
	def run(self):
		self.host = Host(self)
		self.host.read()
		
		self.command = self.read_str()
		argc = self.read_int()
		
		for i in range(argc):
			self.args.append(self.read_str())
		
		try:
			self.module = importlib.import_module("scripts.%s" % self.command)
		except ModuleNotFoundError:
			self.res = 404
		else:
			self.res = 200
		
		self.write_int(self.res)
		self.write_str(self.job_name)
		self.close()
		
		self.lock()
		self.run_job()
		self.unlock()
		
		print("%s (%s) exited -> %s" % (self.job_name, self.pid, self.status))


class Worker:
	running_jobs: dict
	keep_alive: bool
	
	server_sock: socket
	
	def __init__(self):
		self.server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM, 0)
		self.keep_alive = True
		
		self.running_jobs = {}
		self.server_path = "/worker/comm.uds"
		
		try:
			os.unlink(self.server_path)
		except OSError:
			if os.path.exists(self.server_path):
				raise
	
	@staticmethod
	def new_job_name():
		exists = True
		name = ""
		
		while exists:
			name = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(16))
			exists = stat('logs/%s.log' % name) != 0
		
		return name
	
	def start(self):
		# Init the network
		self.server_sock.bind(self.server_path)
		self.server_sock.listen(2)
		
		mkdir("logs")
		
		while self.keep_alive:
			conn, address = self.server_sock.accept()
			
			job_name = Worker.new_job_name()
			self.running_jobs[job_name] = WorkerThread(job_name, conn)
			
			print("%s connected - %s" % (address, job_name))
			self.running_jobs[job_name].start()
		
		print("----Worker exits----")
		self.server_sock.close()

	def close(self):
		self.server_sock.close()
		
		for job in self.running_jobs:
			job.join()


def main():
	signal.signal(signal.SIGINT, signal.SIG_IGN)
	handler = Worker()
	handler.start()


if __name__ == "__main__":
	main()
