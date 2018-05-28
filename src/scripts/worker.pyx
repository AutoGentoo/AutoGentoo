#!/usr/bin/python

import signal
from d_malloc cimport Binary

cdef class Worker:
	def __init__(self, srv: Server):
		self.server = srv
		self.queue = []
	
	cdef parse_queue(self):
		req = Request(REQ_BINQUEUE, PROT_AUTOGENTOO)
		append_to_queue = Binary(self.server.sock.request(req.finish()))
		
		append_to_queue.add_sentinel(QUEUE_END)
		
		while append_to_queue.inside():
			self.queue.append (Job(append_to_queue))
	
	def flush_queue(self, signum, stack):
		self.parse_queue ()
		
		for job in self.queue:
			job.run ()
		self.queue = []
		
		self.start()
	
	def start (self):
		signal.pause()
