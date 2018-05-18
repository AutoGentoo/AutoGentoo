import os, stat
from d_malloc cimport Binary
from interface cimport Server

class shell:
	@staticmethod
	def mv (src, dest):
		os.rename (src, dest)
	
	@staticmethod
	def mkdir (path):
		os.mkdir(path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IROTH | stat.S_IXOTH)
	
	@staticmethod
	def cd (path):
		oldpath = os.getcwd()
		os.chdir(path)
		
		return oldpath
	
	@staticmethod
	def ln (target, link_name):
		if os.path.lexists (link_name):
			os.remove (link_name)
		os.symlink(target, link_name)

cpdef job_makeconf (hostid):
	pass

cpdef job_handoff (stage_id, hostname, profile):
	pass

cdef class Job:
	def __init__ (self, Binary message, Server srv):
		self.srv = srv
		self.message = message
		
		self.objects = []
		self.type = <JobType>self.message.read_int()
		self.template = self.message.read_string()
		for i in str(self.template):
			if i == 'i':
				self.objects.append (<int>self.message.read_int())
			elif i == 's':
				self.objects.append (str(self.message.read_string()))
	
	def run (self):
		job_link[self.type] (*self.objects)
