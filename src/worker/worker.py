import os
import struct
import sys
import _thread
from .client import *
import importlib
from .script import mkdir, touch, rm

WORKER_FIFO_REQUEST = "/tmp/autogentoo_worker.req"
WORKER_FIFO_RESPONSE = "/tmp/autogentoo_worker.res"


class Worker:
	request_fifo = None
	response_fifo = None
	
	read_lck = None
	write_lck = None
	
	pid = None
	running_jobs = {}
	
	keep_alive = True
	
	def __init__(self, server):
		self.server = server
		
		self.request_fifo = open(WORKER_FIFO_REQUEST, "rb")
		self.response_fifo = open(WORKER_FIFO_RESPONSE, "wb")
		
		self.pid = os.getpid()
		self.running_jobs = {}
		self.keep_alive = True
		
		self.read_lck = _thread.allocate_lock()
		self.write_lck = _thread.allocate_lock()
	
	def start(self):
		mkdir("logs")
		
		while self.keep_alive:
			self.read_lck.acquire()
			
			command_enum = self.read_int()
			if command_enum == 1:
				self.read_lck.release()
				# Kill this service
				self.keep_alive = False
				continue
			
			job_name = self.read_str()
			command_name = self.read_str()
			host_id = self.read_str()
			
			argc = self.read_int()
			argv = []
			for i in range(argc):
				argv.append(self.read_str())
			
			self.read_lck.release()
			
			new_job = Job(self.server, self.server.get_host(host_id), job_name, command_name, argv)
			
			self.write_lck.acquire()
			self.write_str(job_name)
			self.write_int(new_job.res)
			self.write_lck.release()
		
		self.request_fifo.close()
		self.response_fifo.close()
	
	def read_int(self):
		return struct.unpack('i', self.request_fifo.read(4))[0]
	
	def read_str(self):
		length = self.read_int()
		
		if length == 0:
			return None
		
		string = self.request_fifo.read(int(length))
		
		return string.decode("utf-8")
	
	def write_int(self, i):
		self.response_fifo.write(i.to_bytes(4, sys.byteorder))
	
	def write_str(self, string):
		self.write_int(len(string))
		self.response_fifo.write(string.encode('utf-8'))


class Job:
	def __init__(self, server, host, job_name, command_name, args):
		self.server = server
		self.host = host
		self.job_name = job_name
		self.command_name = command_name
		self.args = args
		
		try:
			self.module = importlib.import_module("scripts.%s" % self.command_name)
		except ModuleNotFoundError:
			self.res = 1
			return
		
		self.res = 0
		self.pid = 0
	
	def join(self):
		os.waitpid(self.pid)
	
	def run(self):
		logfile = "logs/%s.log" % self.job_name
		touch(logfile + ".lck")
		
		self.pid = os.fork()
		if self.pid == 0:
			logfp = open(logfile, "a+")
			sys.stdout = logfp
			sys.stderr = logfp
			
			# Run the script
			importlib.reload(self.module)
			ret = self.module.script(self.host, self.args)
			
			rm(logfile + ".lck")
			
			logfp.close()
			
			exit(ret)
		
		return 0


def main(argv):
	print("Starting worker from conf file %s" % argv[1])
	
	server = Server(argv[1])
	server.read()
	
	worker = Worker(server)
	worker.start()


if __name__ == "__main__":
	main(sys.argv)
