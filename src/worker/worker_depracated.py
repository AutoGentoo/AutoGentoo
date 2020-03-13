#! /usr/bin/env python

import struct
import sys
import _thread
from client import *
import importlib
from script import *
import signal

logfp = None
WORKER_FIFO_REQUEST = "/tmp/autogentoo_worker.req"
WORKER_FIFO_RESPONSE = "/tmp/autogentoo_worker.res"

WORKER_EXIT = 0
WORKER_JOB = 1


class Worker:
	request_fifo = None
	response_fifo = None
	
	read_lck = None
	write_lck = None
	
	pid = None
	running_jobs = {}
	
	keep_alive = True
	
	def __init__(self, server: Server):
		self.server = server
		
		self.request_fifo = open(WORKER_FIFO_REQUEST, "rb")
		self.response_fifo = open(WORKER_FIFO_RESPONSE, "wb")
		
		self.pid = os.getpid()
		self.jobs = []
		self.keep_alive = True
		
		self.read_lck = _thread.allocate_lock()
		self.write_lck = _thread.allocate_lock()
	
	def start(self):
		mkdir("logs")
		self.server.read()
		
		while self.keep_alive:
			self.read_lck.acquire()
			
			command_enum = self.read_int()
			if command_enum == WORKER_EXIT or command_enum == -1:
				self.read_lck.release()
				# Kill this service
				self.keep_alive = False
				continue
			
			self.server.read()
			
			job_name = self.read_str()
			command_name = self.read_str()
			host_id = self.read_str()
			
			argc = self.read_int()
			argv = []
			for i in range(argc):
				argv.append(self.read_str())
			
			self.read_lck.release()
			
			error = False
			try:
				new_job = Job(self.server, self.server.hosts[host_id], job_name, command_name, argv)
			except KeyError:
				error = True
			else:
				new_job.run()
				self.jobs.append(new_job)
			
			self.write_lck.acquire()
			if error:
				self.write_str("NULL")
				self.write_int(1)
			else:
				self.write_str(job_name)
				self.write_int(new_job.res)
			self.write_lck.release()
		
		print("----Worker exits----")
		self.request_fifo.close()
		self.response_fifo.close()

	def read_int(self):
		buf = self.request_fifo.read(4)
		if len(buf) != 4:
			return -1

		return struct.unpack('i', buf)[0]
	
	def read_str(self):
		length = self.read_int()
		
		if length == 0:
			return None
		
		string = self.request_fifo.read(int(length))
		
		return string.decode("utf-8")
	
	def write_int(self, i):
		self.response_fifo.write(i.to_bytes(4, sys.byteorder))
		self.response_fifo.flush()
	
	def write_str(self, string):
		self.write_int(len(string))
		self.response_fifo.write(string.encode('utf-8'))
		self.response_fifo.flush()
	
	def close(self):
		self.request_fifo.close()
		self.response_fifo.close()
		
		for job in self.jobs:
			job.join()


class Job:
	def __init__(self, server: Server, host: Host, job_name: str, command_name: str, args):
		self.server = server
		self.host = host
		self.job_name = job_name
		self.command_name = command_name
		self.args = args
		
		try:
			self.module = importlib.import_module("scripts.%s" % self.command_name)
		except ModuleNotFoundError:
			self.res = 400
			return
		
		self.res = 200
		self.pid = 0
	
	def join(self):
		os.waitpid(self.pid, 0)
	
	def run(self):
		logfile = "logs/%s.log" % self.job_name
		touch(logfile + ".lck")
		
		# Fork this process to give a response
		self.pid = os.fork()
		if self.pid == 0:
			working_dir = os.getcwd()
			
			# Run the script
			importlib.reload(self.module)
			
			script_pid = os.fork()
			if script_pid == 0:
				global logfp
				logfp = open(logfile, "a+")
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
				pid, status = os.waitpid(script_pid, 0)
				
				cd(working_dir)
				rm(logfile + ".lck")
				
				exit(status)
		
		return 0


def main(argv):
	server = Server(argv[1], int(argv[2]))
	server.read()
	
	worker = Worker(server)
	
	signal.signal(signal.SIGINT, signal.SIG_IGN)
	worker.start()


if __name__ == "__main__":
	main(sys.argv)
