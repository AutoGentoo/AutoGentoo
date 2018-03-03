from op_string cimport CString
from vector cimport PyVec
from op_socket cimport Address, Socket
from libc.string cimport strlen, strdup
from libc.stdlib cimport free

cdef extern from "<autogentoo/writeconfig.h>":
	cdef enum:
		AUTOGENTOO_FILE_END = 0xffffffff,
		AUTOGENTOO_HOST = 0xfffffff0,
		AUTOGENTOO_HOST_BINDING = 0xffffff00,
		AUTOGENTOO_STAGE = 0xfffff000,
		AUTOGENTOO_HOST_END = 0xaaaaaaaa,
		AUTOGENTOO_HOST_KERNEL = 0xbbbbbbbb
	
	ctypedef enum chroot_t:
		CHR_NOT_MOUNTED,
		CHR_MOUNTED
	
	ctypedef enum template_selects:
		OTHER = 0x00,
		CXXFLAGS = 0x01,
		TMPDIR = 0x02,
		PORTDIR = 0x04,
		DISTDIR = 0x08,
		PKGDIR = 0x10,
		LOGDIR = 0x20
	
	int ntohl (int)

cdef class Binary:
	cdef CString buffer
	cdef size_t pos
	
	def __cinit__ (self, char* buffer):
		self.buffer = buffer
		self.pos = 0
	
	cdef char* read_string (self):
		cdef char* out = self.buffer.parent.ptr + self.pos
		self.pos += strlen (out)
		return strdup (out)
	
	cdef int read_int (self):
		cdef int out = (<int *>self.buffer.parent.ptr + self.pos)[0]
		self.pos += sizeof (int)
		return ntohl(out)
	
	cdef void skip_until (self, to_find):
		if sizeof(to_find) == 1:
			to_find = [to_find]
		
		cdef char c = (self.buffer.parent.ptr + self.pos)[0]
		while c not in (*to_find, AUTOGENTOO_FILE_END):
			self.pos += 1
			c = (self.buffer.parent.ptr + self.pos)[0]

cdef class Host
cdef class Stage

cdef class Server:
	cdef readonly Address adr
	cdef char* target;
	
	cdef PyVec hosts
	cdef dict staged;
	cdef sock
	
	def __cinit__ (self, Address adr):
		self.hosts = PyVec ()
		self.stages = {}
		self.adr = adr
		self.sock = Socket (self.adr)
	
	cdef void read_server (self):
		cdef Binary server_bin = Binary (self.sock.request())
	
		cdef Host hbuff
		cdef Stage sbuff
		cdef int current = server_bin.read_int()
		while current != AUTOGENTOO_FILE_END:
			if current == AUTOGENTOO_HOST:
				hbuff = Host (self)
				hbuff.parse (server_bin)
				self.hosts.append (hbuff)
			elif current == AUTOGENTOO_STAGE:
				sbuff = Stage (self)
				sbuff.parse (server_bin)
				self.staged.__setitem__(sbuff.id.decode ("UTF-8"), sbuff)
			else:
				Binary.skip_until((AUTOGENTOO_HOST, AUTOGENTOO_STAGE))
			current = server_bin.read_int()
	
	def __dealloc__ (self):
		free (self.target)
		for i in self.hosts: del i
		for i in self.stages: del self.stages[i]

cdef class Host:
	cdef readonly Server parent # The parent server
	cdef readonly char* id # The ID of the Host
	cdef readonly char* profile # Portage profile, see possible values with eselect profile list
	cdef readonly char* hostname # Hostname of the host (shows up in the graphical client)
	
	cdef readonly char* cflags # The gcc passed to C programs, try -march=native :)
	cdef readonly char* cxxflags # The gcc passed only to CXX programs
	cdef readonly char* arch #  The portage-arch (eg. amd64)
	cdef readonly char* chost # The system chost (should not be changed after it is set)
	cdef readonly char* use # use flags
	cdef readonly PyVec extra # A list of extra entries to go into make.conf
	
	cdef readonly char* portage_tmpdir # build dir
	cdef readonly char* portdir # ebuild portage tree
	cdef readonly char* distdir # distfiles
	cdef readonly char* pkgdir # path to binaries
	cdef readonly char* port_logdir # logs
	cdef readonly chroot_t chroot_status
	cdef readonly PyVec kernel
	
	def __init__ (self, parent):
		self.parent = parent
		self.extra = PyVec ()
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
		self.chroot_status = -1
	
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
				print("Could not understand autogentoo data type: 0x%x", current)
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
		
		for i in self.kernel: del i
		self.extra.free_strings()

cdef class StageExtra:
	cdef char* make_extra
	cdef template_selects select
	
	def __cinit__ (self, char* m_e, template_selects sel):
		self.make_extra = m_e
		self.select = sel
	
	def __dealloc__ (self):
		free (self.make_extra)

cdef class Stage:
	cdef readonly char* id
	cdef readonly char* arch
	cdef readonly char* cflags
	cdef readonly char* chost
	
	cdef readonly PyVec extras
	
	cdef readonly char* dest_dir
	cdef readonly char* parent
	cdef readonly char* new_id
	
	def __init__ (self, parent):
		self.parent = parent
		self.id = NULL
		self.arch = NULL
		self.cflags = NULL
		self.chost = NULL
		self.extra = PyVec ()
		self.dest_dir = NULL
		self.new_id = NULL
	
	def parse (self, Binary _bin):
		self.id = _bin.read_string()
		self.arch = _bin.read_string()
		self.cflags = _bin.read_string()
		self.chost = _bin.read_string()
		
		cdef extra_c = _bin.read_int();
		for i in range (extra_c):
			cdef t_s = _bin.read_string()
			cdef t_i = _bin.read_int()
			self.extra.append (StageExtra (t_s, t_i))
		
		self.dest_dir = _bin.read_string()
		self.new_id = _bin.read_string()
	
	def __dealloc__ (self):
		free (self.id)
		free (self.arch)
		free (self.cflags)
		free (self.chost)
		for i in self.extra:
			del i
		free (self.dest_dir)
		free (self.new_id)