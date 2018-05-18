#!/usr/bin/python

from interface import Server
from job import Job
import signal
from d_malloc import Binary


class Worker:
	def __init__(self, srv: Server):
		self.server = srv
		self.queue = []
		signal.signal(signal.SIGUSR1, self.flush_queue)
	
	def perform_job(self, job: Job):
		pass
	
	def parse_queue(self):
		append_to_queue = Binary(self.server.sock.request())
	
	def flush_queue(self, signum, stack):
		for i in range (len(self.queue), -1, -1):
			self.queue[i]
		
