from op_socket cimport Address, Socket
from libc.stdlib cimport free, malloc
from libc.stdio cimport *
from libc.string cimport *
from log import Log
from request cimport *

cdef extern from "<arpa/inet.h>":
	int htonl (int)

cdef class Server:
	def __init__ (self, Address adr, message=True):
		self.hosts = []
		self.stages = []
		self.templates = []
		self.adr = adr
		self.sock = Socket (self.adr)
		self.message = message
	
	def error (self, mess, *vargs, **kwargs):
		if self.message:
			Log.error (mess, *vargs, **kwargs)
	def info (self, mess, *vargs, **kwargs):
		if self.message:
			Log.info (mess, *vargs, **kwargs)
	
	def new_host (self, int template, str hostname, str profile, str cflags, str use, extra):
		cdef Request n_stage_req = Request (REQ_STAGE_NEW, PROT_AUTOGENTOO)
		n_stage_req.add_templateselect(template)
		
		cdef DynamicBuffer res = self.sock.request(n_stage_req.finish())
		cdef char* new_id_name = strtok (<char*>res.ptr, "\n")
		
		cdef Request stage_req = Request (REQ_STAGE, PROT_AUTOGENTOO)
		stage_req.add_stagecommand(STAGE_ALL)
		
		cdef DynamicBuffer bres = self.sock.request(stage_req.finish(), _print=True, _print_raw=False, _store=False)
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		if check_tr(b"HTTP/1.0 200 OK\n") != 0:
			self.error ("Download or extract failed\n")
			return None
		
		cdef Request handoff_req = Request(REQ_HANDOFF, PROT_AUTOGENTOO)
		handoff_req.add_templateselect(template)
		
		bres = self.sock.request(handoff_req.finish())
		if check_tr(b"HTTP/1.0 200 OK\n") != 0:
			self.error ("Host handoff failed\n")
			return None
		
		self.read_server()
		cdef Host target = self.find_host (new_id_name)
		# 0 hostname
		# 1 profile
		# 2 cflags
		# 3 use
		
		cdef Request edit_req = Request(REQ_HOSTUPLOAD, PROT_AUTOGENTOO)
		edit_req.add_hostupload_str (HOSTOFF_HOSTNAME, hostname.encode('utf-8'))
		edit_req.add_hostupload_str (HOSTOFF_PROFILE, profile.encode('utf-8'))
		edit_req.add_hostupload_str (HOSTOFF_CFLAGS, cflags.encode('utf-8'))
		edit_req.add_hostupload_str (HOSTOFF_USE, use.encode('utf-8'))
		edit_req.add_hostupload_list (HOSTOFF_EXTRA, extra)
		self.sock.request(edit_req.finish())
		
		return new_id_name.decode ("utf-8")
	
	
	def activate (self, str host_id):
		cdef Request req = Request(REQ_ACTIVATE, PROT_AUTOGENTOO)
		req.add_hostselect(host_id.encode("utf-8"))
		
		cdef DynamicBuffer bres = self.sock.request(req.finish())
		
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		
		if check_tr (b"HTTP/1.0 200 OK\n") == 0:
			return True
		return False
	
	def install (self, str _str):
		temp = _str.encode("utf-8")
		cdef char* c_str = temp
		
		cdef Request req = Request (REQ_INSTALL, PROT_AUTOGENTOO)
		req.add_hostinstall(c_str)
		
		cdef DynamicBuffer bres = self.sock.request(req.finish(), _print=True, _print_raw=False, _store=False)
		
		check_tr = lambda s: strncmp (<char*>bres.ptr + (bres.n - len(s)), s, len(s))
		
		
		if check_tr (b"HTTP/1.0 200 OK\n") == 0:
			return True
		elif check_tr (b"HTTP/1.0 403 Forbidden\n") == 0:
			self.error("You must select a build host first\n")
		elif check_tr (b"HTTP/1.0 504 Chroot Not Mounted\n") == 0:
			if strncmp (<char*>self.sock.request(Request(REQ_MNTCHROOT, PROT_AUTOGENTOO).finish()).ptr, "HTTP/1.0 2", 10) == 0:
				return self.install (_str)
			self.error("Failed to mount chroot\n")
			self.error("Check the server selfs\n")
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
			("PORT_selfDIR", LOGDIR),
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
		
		if strncmp (<char*>self.sock.request(req.finish()).ptr, "HTTP/1.0 200 OK", 15) == 0:
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
		
		cdef Binary server_bin = Binary (self.sock.request(Request(REQ_BINSERVER, PROT_AUTOGENTOO).finish() ))
		
		p_a = False
		p_h = False
		p_s = False
		p_t = False
		
		cdef int current = server_bin.read_int()
		while current != AUTOGENTOO_FILE_END and server_bin.inside():
			if current == AUTOGENTOO_HOST:
				if not p_h:
					if p_a:
						self.info ("ok", color_only=True, newline=True)
					self.info ("Processing hosts...", flush=True)
					p_a = True
					p_h = True
				hbuff = Host (self)
				hbuff.parse(server_bin)
				self.hosts.append (hbuff)
			elif current == AUTOGENTOO_STAGE:
				if not p_s:
					if p_a:
						self.info ("ok", color_only=True, newline=True)
					self.info ("Processing stages...", flush=True)
					p_s = True
					p_a = True
				sbuff = Stage (self)
				sbuff.parse (server_bin)
				self.stages.append(sbuff)
			elif current == AUTOGENTOO_TEMPLATE:
				if not p_t:
					if p_a:
						self.info ("ok", color_only=True, newline=True)
					self.info ("Processing templates...", flush=True)
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
			self.info ("ok", color_only=True, newline=True)
	

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
		self.extra_c = 0
	
	def set_field (self, f1, f2, value):
		cdef Request request = Request (REQ_EDIT, PROT_AUTOGENTOO)
		request.add_hostselect(self.id)
		request.add_hostedit(f1, f2, value.encode('utf8'))
		self.parent.sock.request (request.finish())
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
		
		self.extra_c = _bin.read_int()
		self.extra = <char**>malloc (sizeof(char*) * (self.extra_c + 1))
		for i in range (self.extra_c):
			self.extra[i] = _bin.read_string()
		self.extra[self.extra_c] = NULL
		
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
				self.parent.error ("Could not understand autogentoo data type: 0x%x\n", current)
				break
			current = _bin.read_int()
	
	def upload (self):
		cdef DynamicBuffer b = DynamicBuffer ()
		b.append_string(self.id)
		b.append_int(self.chroot_status)
		b.append_string(self.hostname)
		b.append_string(self.profile)
		b.append_string(self.cflags)
		b.append_string(self.cxxflags)
		b.append_string(self.use)
		b.append_string(self.arch)
		b.append_string(self.chost)
		b.append_int(self.extra_c)
		
		for i in range (self.extra_c):
			b.append_string(self.extra[i])
		
		b.append_string(self.portage_tmpdir)
		b.append_string(self.portdir)
		b.append_string(self.pkgdir)
		b.append_string(self.port_logdir)
		b.append_int(AUTOGENTOO_HOST_END)
	
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
	
	cdef char* find_extra (self, template_selects key, char* default):
		for i in range(self.extra_c):
			if self.extra[i].select == key:
				return self.extra[i].make_extra
		return default
	
	cdef Host handoff (self, char* hostname, char* profile, char* use):
		cdef DynamicBuffer temp = DynamicBuffer ()
		temp.append_string(self.new_id)
		temp.append_int (CHR_NOT_MOUNTED)
		temp.append_string(hostname)
		temp.append_string(profile)
		temp.append_string(self.cflags)
		temp.append_string(self.find_extra (CXXFLAGS, "${CFLAGS}"))
		temp.append_string(use)
		temp.append_string(self.arch)
		temp.append_string(self.chost)
		temp.append_int(self.extra_c)
		for i in range (self.extra_c):
			if self.extra[i].select == OTHER:
				temp.append_string(self.extra[i].make_extra)
		
		temp.append_string(self.find_extra (TMPDIR, "/autogentoo/tmp"))
		temp.append_string(self.find_extra (PORTDIR, "/usr/portage"))
		temp.append_string(self.find_extra (DISTDIR, "/usr/portage/distfiles"))
		temp.append_string(self.find_extra (PKGDIR, "/autogentoo/pkg"))
		temp.append_string(self.find_extra (LOGDIR, "/autogentoo/log"))
		
		temp.append_int (AUTOGENTOO_HOST_END)
		
		cdef Binary _bin = Binary (temp)
		
		cdef Host out = Host (self.parent)
		out.parse (_bin)
		return out
	
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
