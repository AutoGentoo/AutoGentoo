#!/usr/bin/python

from interface import Server
from job import Job


class Worker:
	def __init__(self, srv: Server):
		self.server = srv
		self.queue = []
	
	def perform_job (self, job: Job):
		pass
