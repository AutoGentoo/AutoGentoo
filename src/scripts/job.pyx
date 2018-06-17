import os, stat
from d_malloc cimport Binary
from interface cimport Server, Host

class Shell:
	def __init__ (self, host_path):
		if host_path[-1] == "/":
			host_path = host_path[:-1]
		self.path = host_path
	
	def mv (self, src, dest):
		os.rename (self.path + src, self.path + dest)
	
	def mkdir (self, path):
		os.mkdir(self.path + path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IROTH | stat.S_IXOTH)
	
	def cd (self, path):
		oldpath = os.getcwd().replace (self.path, "")
		if path[0] != "/":
			path = oldpath + path
		
		os.chdir(self, self.path + path)
		return oldpath
	
	def ln (self, target, link_name):
		if os.path.lexists (self.path + link_name):
			os.remove (self.path + link_name)
		
		os.symlink(target, self.path + link_name) # Target is relatve and will auto-correct in chroot

cpdef job_makeconf (Server srv, char* hostid):
	cdef Host target = srv.find_host(hostid)
	cdef Shell sh = Shell (os.path.realpath(srv.))
	char buf[PATH_MAX];
	char* dest_temp = realpath(host->parent->location, buf);
	if (dest_temp == NULL) {
		lerror("Failed to get realpath()");
		return;
	}
	
	asprintf(dest, "%s/%s", dest_temp, host->id);

cpdef job_handoff (Server srv, stage_id, hostname, profile):
	pass

cdef class Job:
	def __init__ (self, Binary message, Server srv):
		self.srv = srv
		self.message = message
		
		self.type = <queue_t>self.message.read_int()
		self.template = self.message.read_string()
		self.objects = message.read_template(self.template)
	
	def run (self):
		job_link[self.type] (self.srv, *self.objects)
