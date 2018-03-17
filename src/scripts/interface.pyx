from op_socket cimport Address, Socket, print_raw
from libc.stdlib cimport free, malloc
from libc.stdio cimport *
from libc.string cimport *
from d_malloc cimport DynamicBuffer
from log import Log

cdef extern from "<arpa/inet.h>":
	int htonl (int)

cdef class Server:
	def __init__ (self, Address adr):
		self.hosts = []
		self.stages = {}
		self.templates = []
		self.adr = adr
		self.sock = Socket (self.adr)
	
	cpdef void new_host (self, list fields):
		req = "SRV NEW 0\n" + "\n".join (fields)
		self.sock.request(req.encode('utf8'), _print=True)
	
	def new_template (self, name, arch, chost, cflags, make_conf_entry=()):
		send = [
			name,
			arch,
			cflags,
			chost,
			len(make_conf_entry)
		]
		
		cdef DynamicBuffer out_data = DynamicBuffer ()
		cdef char* req = b"SRV TEMPLATE_CREATE HTTP/1.0\n"
		out_data.append (req, strlen (req))
		
		cdef char* temp_str
		cdef int temp_int
		
		for item in send:
			if isinstance(item, int):
				temp_int = htonl(<int>item)
				out_data.append (&temp_int, sizeof (int))
			else:
				k = item.encode ("utf-8")
				temp_str = k
				out_data.append (temp_str, len(item) + 1)
		
		port_overwrites = [
			("CXXFLAGS", CXXFLAGS),
			("PORTDIR", PORTDIR),
			("PORTAGE_TMPDIR", TMPDIR),
			("DISTDIR", DISTDIR),
			("PKGDIR", PKGDIR),
			("PORT_LOGDIR", LOGDIR),
		]
		
		for item in make_conf_entry:
			temp_int = OTHER
			overwrite = True
			for x in port_overwrites:
				if item.startswith(x[0]):
					temp_int = x[1]
					if input("overwrite %s (Y/n): " % x[0])[0].lower() == "n":
						overwrite = False
					break
			
			if not overwrite:
				continue
			if temp_int != OTHER:
				item = item[item.find("=") + 2: -1]
			
			k = item.encode ("utf-8")
			temp_str = k
			out_data.append(temp_str, strlen(temp_str) + 1)
			
			temp_int = htonl(temp_int)
			out_data.append(&temp_int, sizeof (int))
		
		
		if strncmp (<char*>self.sock.request(out_data).ptr, "HTTP/1.0 200 OK", 15) == 0:
			return name
		return None
	
	
	cpdef void read_server (self):
		self.hosts = []
		self.templates = []
		
		cdef Binary server_bin = Binary (self.sock.request(DynamicBuffer(b"BIN SERVER\n")))
		
		p_a = False
		p_h = False
		p_s = False
		p_t = False
		
		cdef int current = server_bin.read_int()
		while current != AUTOGENTOO_FILE_END and server_bin.inside():
			if current == AUTOGENTOO_HOST:
				if not p_h:
					if p_a:
						Log.info ("ok", color_only=True, newline=True)
					Log.info ("Processing hosts...", flush=True)
					p_a = True
					p_h = True
				hbuff = Host (self)
				hbuff.parse(server_bin)
				self.hosts.append (hbuff)
			elif current == AUTOGENTOO_STAGE:
				if not p_s:
					if p_a:
						Log.info ("ok", color_only=True, newline=True)
					Log.info ("Processing stages...", flush=True)
					p_s = True
					p_a = True
				sbuff = Stage (self)
				sbuff.parse (server_bin)
				self.stages.__setitem__(sbuff.id.decode ("UTF-8"), sbuff)
			elif current == AUTOGENTOO_TEMPLATE:
				if not p_t:
					if p_a:
						Log.info ("ok", color_only=True, newline=True)
					Log.info ("Processing templates...", flush=True)
					p_t = True
					p_a = True
				__sbuff = Stage (self)
				__sbuff.parse (server_bin, True)
				self.templates.append (__sbuff)
			else:
				if not server_bin.skip_until((AUTOGENTOO_HOST, AUTOGENTOO_STAGE, AUTOGENTOO_TEMPLATE)):
					break
			current = server_bin.read_int()
			
		if p_a:
			Log.info ("ok", color_only=True, newline=True)
	

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
	
	def set_field (self, f1, f2, value):
		cdef DynamicBuffer request = DynamicBuffer ()
		cdef char* n = "SRV EDIT\n"
		request.append (n, strlen(n))
		request.append (self.id, 16)
		
		cdef int c_f1 = htonl (<int>f1)
		cdef int c_f2 = <int>f2
		
		request.append (&c_f1, sizeof (int))
		if c_f2 >= 0:
			c_f2 = htonl(c_f2)
			request.append(&c_f2, sizeof (int))
		
		cdef field_set = (<unicode>value).encode('utf8')
		request.append (<char*>field_set, strlen(field_set))
		print_raw(request.ptr, request.n)
		self.parent.sock.request (request, _print=True)
	
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
		self.extra_c = -1
		self.chost = NULL
		self.extra = NULL
		self.dest_dir = NULL
		self.new_id = NULL
	
	def get_extra (self):
		out = []
		for i in range (arr_len(<void**>self.extra)):
			out.append ((self.extra[i].make_extra.decode ("UTF-8"), self.extra[i].select))
		return out
	
	cdef parse (self, Binary _bin, template=False):
		self.id = _bin.read_string()
		self.arch = _bin.read_string()
		self.cflags = _bin.read_string()
		self.chost = _bin.read_string()
		
		self.extra_c = _bin.read_int()
		self.extra = <StageExtra**>malloc (sizeof (StageExtra*) * (self.extra_c + 1))
		
		cdef char* k
		cdef int _k
		
		for i in range (self.extra_c):
			k = _bin.read_string()
			_k = _bin.read_int()
			self.extra[i] = stage_extra (k, <template_selects>_k)
		self.extra[self.extra_c] = NULL
		
		if not template:
			self.dest_dir = _bin.read_string()
			self.new_id = _bin.read_string()
	
	cdef char* send_dup (self, char* cflags=NULL):
		cdef DynamicBuffer out = DynamicBuffer
		out.append (self.id, strlen (id))
		out.append (self.arch, strlen(self.arch))
		
		if cflags is None:
			cflags = self.cflags
		out.append (cflags, strlen(cflags))
		out.append (self.chost, strlen(self.chost))
		out.append (&self.extra_c, sizeof (int))
		
		for i in range (self.extra_c):
			out.append (self.extra[i].make_extra, strlen(self.extra[i].make_extra))
			out.append (&self.extra[i].select, sizeof(int))
	
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
