#!/usr/bin/python

import signal
from d_malloc cimport Binary
from libc.stdio cimport printf, fflush, stdout

cdef class Worker:
	def __init__(self, srv: Server):
		self.server = srv
		self.queue = []
		main_worker = self
	
	cdef parse_queue(self):
		req = Request(REQ_BINQUEUE, PROT_AUTOGENTOO)
		cdef DynamicBuffer k = self.server.sock.request(req.finish())
		append_to_queue = Binary(k)
		append_to_queue.add_sentinel(QUEUE_END)
		
		while append_to_queue.inside():
			printf ("-")
			fflush (stdout)
			self.queue.append (Job(append_to_queue, self.server))
	
	cpdef flush_queue(self):
		printf ("in flush")
		fflush (stdout)
		
		self.parse_queue ()
		printf ("1")
		fflush (stdout)
		
		for job in self.queue:
			job.run ()
		self.queue = []
		
		self.start()
	
	def start (self):
		signal.pause()
