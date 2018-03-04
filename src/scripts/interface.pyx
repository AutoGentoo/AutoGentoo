from op_socket cimport Address, Socket
from libc.stdlib cimport free, malloc
from libc.stdio cimport *
from libc.string cimport *
from d_malloc cimport DynamicBuffer
from log import Log

cdef class Server:
	def __init__ (self, Address adr):
		self.hosts = []
		self.stages = {}
		self.templates = NULL
		self.adr = adr
		self.sock = Socket (self.adr)
	
	cpdef void read_server (self):
		cdef Binary server_bin = Binary (self.sock.request_raw("BIN SERVER\n"))
		
		Log.info ("Processing hosts...", flush=True)
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
		Log.info ("ok", color_only=True, newline=True)
		Log.info ("Processing templates...", flush=True)
		cdef DynamicBuffer template_t = self.sock.request_raw("SRV GETTEMPLATES")
		cdef char* len_buff = strtok (template_t.ptr, "\n")
		cdef int l;
		sscanf (len_buff, "%d", &l)
		
		if self.templates != NULL:
			free_array (<void**>self.templates)
		self.templates = <char**>malloc (sizeof(char*) * l)
		
		for i in range (l):
			self.templates[i] = strdup (strtok (NULL, "\n"))
		self.templates[l] = NULL
		Log.info ("ok", color_only=True, newline=True, flush=True)
	
	def __dealloc__ (self):
		if self.templates != NULL:
			free_array(<void**>self.templates)

cdef class Host(PyOb):
	def __init__ (self, parent):
		self.parent = parent
		self.extra = NULL
		#self.kernel = PyVec ()
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
		self.extra = <char**>malloc (sizeof(char*) * (n + 1))
		for i in range (n):
			self.extra[i] = _bin.read_string()
		self.extra[n] = NULL
		
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
	
	def get_extra (self):
		out = []
		for i in range (arr_len(<void**>self.extra)):
			out.append (self.extra[i].decode ("UTF-8"))
		return out
	
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
		
		free_array (<void**>self.extra)
		#del self.kernel

cdef StageExtra* stage_extra (char* m_e, template_selects sel):
	cdef StageExtra* out = <StageExtra*>malloc (sizeof (StageExtra))
	out.make_extra = m_e
	out.select = sel
	return out

cdef free_array (void** array):
	cdef int i = 0
	cdef void* c = array[i];
	while c != NULL:
		free (c)
		i += 1
		c = array[i]
	free (array)

cdef int arr_len (void** array):
	cdef int i = 0
	cdef void* c = array[i];
	while c != NULL:
		i += 1
		c = array[i]
	return i

cdef class Stage(PyOb):
	def __init__ (self, parent):
		self.parent = parent
		self.id = NULL
		self.arch = NULL
		self.cflags = NULL
		self.chost = NULL
		self.extra = NULL
		self.dest_dir = NULL
		self.new_id = NULL
	
	def get_extra (self):
		out = []
		for i in range (arr_len(<void**>self.extra)):
			out.append ((self.extra[i].make_extra.decode ("UTF-8"), self.extra[i].select))
		return out
	
	cdef parse (self, Binary _bin):
		self.id = _bin.read_string()
		self.arch = _bin.read_string()
		self.cflags = _bin.read_string()
		self.chost = _bin.read_string()
		
		cdef extra_c = _bin.read_int();
		self.extra = <StageExtra**>malloc (sizeof (StageExtra*) * (extra_c + 1))
		for i in range (extra_c):
			self.extra[i] = stage_extra (_bin.read_string(), <template_selects>_bin.read_int())
		self.extra[extra_c] = NULL
		
		self.dest_dir = _bin.read_string()
		self.new_id = _bin.read_string()
	
	def __dealloc__ (self):
		free (self.id)
		free (self.arch)
		free (self.cflags)
		free (self.chost)
		free (self.dest_dir)
		free (self.new_id)
		
		for i in range (arr_len (<void**>self.extra)):
			free (self.extra[i].make_extra)
		free_array (<void**>self.extra)

cdef class PyOb:
	def __init__ (self):
		pass
	
	def get (self, ident):
		return self.__getattribute__(ident).decode ("UTF-8")

