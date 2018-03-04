from vector cimport PyVec
from op_socket cimport Address, Socket
from libc.stdlib cimport free, malloc
from libc.stdio cimport printf, sscanf
from libc.string cimport *
from d_malloc cimport DynamicBuffer

import gc
gc.disable ()

cdef class Server:
	def __init__ (self, Address adr):
		self.hosts = []
		self.stages = PyVec(free_data=True)
		self.templates = []
		self.adr = adr
		self.sock = Socket (self.adr)
	
	cpdef void read_server (self):
		cdef Binary server_bin = Binary (self.sock.request_raw("BIN SERVER\n"))
		
		cdef int current = server_bin.read_int()
		while current != AUTOGENTOO_FILE_END and server_bin.inside():
			if current == AUTOGENTOO_HOST:
				hbuff = Host (self)
				hbuff.parse(server_bin)
				self.hosts.append (hbuff)
			elif current == AUTOGENTOO_STAGE:
				sbuff = Stage (self)
				sbuff.parse (server_bin)
				self.stages.__setitem__(sbuff.id.decode ("UTF-8"), sbuff)
			else:
				if not server_bin.skip_until((AUTOGENTOO_HOST, AUTOGENTOO_STAGE)):
					break
			current = server_bin.read_int()
		
		cdef DynamicBuffer template_t = self.sock.request_raw("SRV GETTEMPLATES")
		buffer = template_t.ptr.decode ("UTF-8")
		buffer = buffer.split ("\n")
		print (buffer)
		
		for i in range (int (buffer[0])):
			print (buffer[i + 1])
			self.templates.append (buffer[i + 1])

cdef class Host(object):
	def __init__ (self, parent):
		self.parent = parent
		self.extra = PyVec (free_data=True)
		self.kernel = PyVec ()
		self.id = NULL
		self.profile = NULL
		self.hostname = NULL
		self.cflags = NULL
		self.cxxflags = NULL
		self.arch = NULL
		self.chost = NULL
		self.use = NULL
		self.portage_tmpdir = NULL
		self.portdir = NULL
		self.distdir = NULL
		self.pkgdir = NULL
		self.port_logdir = NULL
		self.chroot_status = <chroot_t>-1
	
	cdef void parse (self, Binary _bin):
		self.id = _bin.read_string()
		self.chroot_status = <chroot_t>_bin.read_int()
		
		self.hostname = _bin.read_string()
		self.profile = _bin.read_string()
		self.cflags = _bin.read_string()
		self.cxxflags = _bin.read_string()
		self.use = _bin.read_string()
		self.arch = _bin.read_string()
		self.chost = _bin.read_string()
		
		n = _bin.read_int()
		for i in range (n):
			self.extra.append (_bin.read_string())
		
		self.portage_tmpdir = _bin.read_string()
		self.portdir = _bin.read_string()
		self.distdir = _bin.read_string()
		self.pkgdir = _bin.read_string()
		self.port_logdir = _bin.read_string()
		
		cdef int current = _bin.read_int();
		while current != AUTOGENTOO_HOST_END:
			if current == AUTOGENTOO_HOST_KERNEL:
				pass
			else:
				printf("Could not understand autogentoo data type: 0x%x", current)
				break
			current = _bin.read_int()
	
	def __dealloc__ (self):
		free(self.id)
		free(self.hostname)
		free(self.profile)
		free(self.cflags)
		free(self.cxxflags)
		free(self.use)
		free(self.chost)
		free(self.arch)
		free(self.portage_tmpdir)
		free(self.portdir)
		free(self.distdir)
		free(self.pkgdir)
		free(self.port_logdir)
		
		del self.extra
		del self.kernel

cdef StageExtra* stage_extra (char* m_e, template_selects sel):
	cdef StageExtra* out = <StageExtra*>malloc (sizeof (StageExtra))
	out.make_extra = m_e
	out.select = sel
	return out

cdef class Stage:
	def __init__ (self, parent):
		self.parent = parent
		self.id = NULL
		self.arch = NULL
		self.cflags = NULL
		self.chost = NULL
		self.extra = PyVec (free_data=True)
		self.dest_dir = NULL
		self.new_id = NULL
	
	cdef parse (self, Binary _bin):
		self.id = _bin.read_string()
		self.arch = _bin.read_string()
		self.cflags = _bin.read_string()
		self.chost = _bin.read_string()
		
		cdef extra_c = _bin.read_int();
		for i in range (extra_c):
			self.extra.append (<object>stage_extra (_bin.read_string(), <template_selects>_bin.read_int()))
		
		self.dest_dir = _bin.read_string()
		self.new_id = _bin.read_string()
	
	def __dealloc__ (self):
		free (self.id)
		free (self.arch)
		free (self.cflags)
		free (self.chost)
		free (self.dest_dir)
		free (self.new_id)
		
		for i in range (self.extra.size):
			free ((<StageExtra*>(<void*>self.extra.get (i))).make_extra)
		
		del self.extra