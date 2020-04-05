#! /usr/bin/env python
import string
import sys
import threading
import importlib
import signal
import socket
import random
from typing import Union

from api.dynamic_binary import SockStream
from script import *
from client import Host
import scripts.emerge, scripts.make_conf, scripts.package_etc


class WorkerThread(threading.Thread, SockStream):
	logfile: str
	job_name: str
	working_dir: str
	pid: int
	status: int
	
	host: Union[Host, None]
	command: str
	arg: str
	res: int
	
	def __init__(self, job_name: str, client: socket.socket):
		threading.Thread.__init__(self)
		SockStream.__init__(self, client)
		
		self.job_name = job_name
		self.working_dir = os.getcwd()
		self.module = None
		self.status = -1
		self.res = -1
		self.pid = 0
		self.logfile = "/autogentoo/jobs/%s.log" % job_name
		
		self.args = ""
	
	def lock(self):
		cwd = pwd
		cd(self.working_dir)
		touch(self.logfile + ".lck")
		cd(cwd)
	
	def unlock(self):
		cwd = pwd
		cd(self.working_dir)
		rm(self.logfile + ".lck")
		cd(cwd)
	
	def run_job(self):
		self.pid = os.fork()
		if self.pid == -1:
			return -1
		elif self.pid == 0:
			# Run the script
			
			logfp = open(self.logfile, "a+")
			sys.stdout = logfp
			sys.stderr = logfp
			
			try:
				ret = self.module.script(self.job_name, self.host, self.arg)
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
		self.read_data()
		
		s = self.read_int()
		print(s)
		
		self.host = Host(self)
		
		try:
			self.host.read()
		except:
			traceback.print_exc(file=sys.stdout)
			self.res = 400
		
		self.command = self.read_str()
		self.arg = self.read_str()
		
		if self.res != 400:
			try:
				self.module = eval("scripts.%s" % self.command)
				importlib.reload(self.module)
			except:
				print("Failed to load module scripts.%s" % self.command)
				self.res = 404
			else:
				self.res = 200
		
		self.write_int(self.res)
		self.write_str(self.job_name)
		self.close()
		
		if self.res == 200:
			self.lock()
			self.res = self.run_job()
			self.unlock()
		
		print("%s exited -> %s" % (self.job_name, self.res))


class Worker:
	running_jobs: dict
	keep_alive: bool
	
	server_sock: socket
	
	def __init__(self):
		self.server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM, 0)
		self.keep_alive = True
		
		self.running_jobs = {}
		self.server_path = "/autogentoo/comm.uds"
		
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
			exists = stat('/autogentoo/jobs/%s.log' % name) != 0
		
		return name
	
	def start(self):
		# Init the network
		self.server_sock.bind(self.server_path)
		self.server_sock.listen(2)
		
		mkdir("/autogentoo/jobs/")
		
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
	handler = Worker()
	handler.start()


if __name__ == "__main__":
	main()
