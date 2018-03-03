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
	cdef Address adr
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
	
		cdef Host hbuff;
		cdef int current = server_bin.read_int();
		while current != AUTOGENTOO_FILE_END:
			if current == AUTOGENTOO_HOST:
				hbuff = Host (self)
				hbuff.parse (server_bin)
				self.hosts.append (hbuff)
			elif current == AUTOGENTOO_STAGE:
				pass
			else:
				Binary.skip_until((AUTOGENTOO_HOST, AUTOGENTOO_STAGE))
			current = server_bin.read_int()
	
	def __dealloc__ (self):
		free (self.target)
		for i in self.hosts: del i

cdef class Host:
	cdef Server parent # The parent server
	cdef char* id # The ID of the Host
	cdef char* profile # Portage profile, see possible values with eselect profile list
	cdef char* hostname # Hostname of the host (shows up in the graphical client)
	
	cdef char* cflags # The gcc passed to C programs, try -march=native :)
	cdef char* cxxflags # The gcc passed only to CXX programs
	cdef char* arch #  The portage-arch (eg. amd64)
	cdef char* chost # The system chost (should not be changed after it is set)
	cdef char* use # use flags
	cdef PyVec extra # A list of extra entries to go into make.conf
	
	cdef char* portage_tmpdir # build dir
	cdef char* portdir # ebuild portage tree
	cdef char* distdir # distfiles
	cdef char* pkgdir # path to binaries
	cdef char* port_logdir # logs
	cdef chroot_t chroot_status
	cdef PyVec kernel
	
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

cdef class Stage:
	cdef char* id;
