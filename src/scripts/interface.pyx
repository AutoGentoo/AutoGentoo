from op_socket cimport Address, Socket
from libc.stdlib cimport free, malloc
from libc.stdio cimport *
from libc.string cimport *
from log import Log
from request cimport *

cdef extern from "<arpa/inet.h>":
	int htonl (int)

cdef class Server:
	def __init__ (self, Address adr):
		self.hosts = []
		self.stages = []
		self.templates = []
		self.adr = adr
		self.sock = Socket (self.adr)
	
	def new_host (self, Stage template, str hostname, str profile, str cflags, str use):
		cdef DynamicBuffer n_stage_req = DynamicBuffer (b"SRV STAGE_NEW ")
		n_stage_req.append (<char*>template.id, strlen (template.id))
		n_stage_req.append (b"\n", 1)
		
		cdef DynamicBuffer res = self.sock.request(n_stage_req)
		cdef char* new_id_name = strtok (<char*>res.ptr, "\n")
		
		cdef DynamicBuffer stage_req = DynamicBuffer (b"SRV STAGE ")
		stage_req.append (new_id_name, strlen (new_id_name))
		stage_req.append (b"\n", 1)
		stage_req.append (b"ALL\n", 4)
		
		cdef DynamicBuffer bres = self.sock.request(stage_req, _print=True, _print_raw=False, _store=False)
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		if check_tr(b"HTTP/1.0 200 OK\n") != 0:
			Log.error ("Download or extract failed\n")
			return None
		
		cdef DynamicBuffer handoff_req = DynamicBuffer(b"SRV HANDOFF ")
		stage_req.append (new_id_name, strlen (new_id_name))
		stage_req.append (b"\n", 1)
		bref = self.sock.request(handoff_req)
		if check_tr(b"HTTP/1.0 200 OK\n") != 0:
			Log.error ("Host handoff failed\n")
			return None
		
		self.read_server()
		cdef Host target = self.find_host (new_id_name)
		# 0 hostname
		# 1 profile
		# 2 cflags
		# 3 use
		
		target.set_field(0, -1, hostname)
		target.set_field(1, -1, profile)
		target.set_field(2, -1, cflags)
		target.set_field(3, -1, use)
		
		return new_id_name.decode ("utf-8")
	
	
	def activate (self, str host_id):
		cdef DynamicBuffer bres = self.sock.request(DynamicBuffer(("SRV ACTIVATE " + host_id).encode ("utf-8")))
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		
		if check_tr (b"HTTP/1.0 200 OK\n") == 0:
			return True
		return False
	
	def install (self, str _str):
		temp = _str.encode("utf-8")
		cdef char* c_str = temp
		
		cdef DynamicBuffer req = DynamicBuffer ()
		req.append (b"INSTALL\n", 8)
		req.append (c_str, strlen (c_str))
		
		cdef DynamicBuffer bres = self.sock.request(req, _print=True, _print_raw=False, _store=False)
		
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		
		
		if check_tr (b"HTTP/1.0 200 OK\n") == 0:
			return True
		elif check_tr (b"HTTP/1.0 403 Forbidden\n") == 0:
			Log.error("You must select a build host first\n")
		elif check_tr (b"HTTP/1.0 504 Chroot Not Mounted\n") == 0:
			if strncmp (<char*>self.sock.request(DynamicBuffer(b"SRV MNTCHROOT\n")).ptr, "HTTP/1.0 2", 10) == 0:
				return self.install (_str)
			Log.error("Failed to mount chroot\n")
			Log.error("Check the server logs\n")
			return False
		
		return True
	
	def new_template (self, name, arch, chost, cflags, make_conf_entry=()):
		send = [
			name.encode('utf-8'),
			arch.encode('utf-8'),
			cflags.encode('utf-8'),
			chost.encode('utf-8')
		]
		
		port_overwrites = [
			("CXXFLAGS", CXXFLAGS),
			("PORTDIR", PORTDIR),
			("PORTAGE_TMPDIR", TMPDIR),
			("DISTDIR", DISTDIR),
			("PKGDIR", PKGDIR),
			("PORT_LOGDIR", LOGDIR),
		]
		
		me_out = []
		
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
			
			temp_str  = item.encode ("utf-8")
			me_out.append (MakeExtra(temp_str, temp_int))
		
		cdef Request req = Request (REQ_TEMPLATE_CREATE, PROT_AUTOGENTOO)
		req.add_templatecreate(send[0], arch[1], cflags[2], chost[3], me_out)
		
		if strncmp (<char*>self.sock.request(req.data).ptr, "HTTP/1.0 200 OK", 15) == 0:
			self.read_server()
			return name
		return None
	
	cdef Host find_host (self, char* name):
		for x in self.hosts:
			if strcmp (name, x.id):
				return x
		return None
	
	cpdef void read_server (self):
		self.hosts = []
		self.templates = []
		self.stages = []
		
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
				self.stages.append(sbuff)
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
		super ().__init__()
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
		cdef Request request = Request (REQ_EDIT, PROT_AUTOGENTOO)
		
		request.add_hostselect(self.id)
		request.add_hostedit(f1, f2, (<unicode>value).encode('utf8'))
		
		self.parent.sock.request (request.data)
		self.parent.read_server()
	
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
		super ().__init__()
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
		cdef DynamicBuffer out = DynamicBuffer ()
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
