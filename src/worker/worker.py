#! /usr/bin/env python
import signal
import sys
from script import *
import socket
from log import Log
from request import Request

WORKER_SOCK_UDS = "/autogentoo/comm.uds"
WORKER_LOG = "/autogentoo/worker.log"

WORKER_EXIT = 0
WORKER_JOB = 1


class Worker:
	server_uds: socket.socket
	
	pid = None
	running_jobs = {}
	log: Log
	
	keep_alive = True
	
	def __init__(self):
		self.job_queue = []
		self.pid = os.getpid()  # should be 1
		self.log = Log(WORKER_LOG)
		
		self.server_uds = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM, 0)
		self.bind()
		self.listen()
	
	def bind(self):
		try:
			self.server_uds.bind("WORKER_SOCK_UDS")
		except Error as e:
			self.log.error(e.strerror, e.errno)
		
	def listen(self):
		try:
			self.server_uds.listen(32)
		except Error as e:
			self.log.error(e.strerror, e.errno)
	
	def start(self):
		mkdir("/autogentoo/logs")
		
		while self.keep_alive:
			conn, adr = self.server_uds.accept()
			if conn is None:
				continue
			
			self.log.info("%s has connected" % adr)
			
			request = Request(conn, self.log, self.request_exit)
			err = request.handle()
			
			if err == 0:
				self.log.info("Job #%d starting" % request.jobnum)
				self.running_jobs[request.jobnum] = request
				request.start()
			else:
				self.log.error("An error occured in processing request")
	
	def request_exit(self, request: Request):
		del self.running_jobs[request]
	
	def get_jobs(self):
		for key in self.running_jobs:
			yield key, self.running_jobs[key]


def main(argv):
	worker = Worker()
	
	signal.signal(signal.SIGINT, signal.SIG_IGN)
	worker.start()


if __name__ == "__main__":
	main(sys.argv)
