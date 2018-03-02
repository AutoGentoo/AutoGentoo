from libc.stdlib cimport atoi
import socket
from op_string import CString
from vector import PyVec

cdef class Host
cdef class Stage

cdef class Address:
	cdef char port[4]
	cdef char* ip

cdef class Server:
	cdef Address adr
	cdef char* target;
	
	cdef PyVec hosts;
	cdef staged;
	
	def __cinit__ (self, Address adr):
		self.hosts = PyVec (sizeof(Host*))
		self.stages = {}
		self.adr = adr
	
	cdef void cython_read_server (self, CString buffer):
		pass

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
	cdef char[:]* extra # A list of extra entries to go into make.conf
	
	cdef char* portage_tmpdir # build dir
	cdef char* portdir # ebuild portage tree
	cdef char* distdir # distfiles
	cdef char* pkgdir # path to binaries
	cdef char* port_logdir # logs

cdef class Stage:
	cdef char* id;

cdef socket_request (Address adr, char* request, void (*func)):
	cdef int portno = atoi (adr.port);
	sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
	
	sock.connect ((adr.ip.decode("UTF-8"), portno))
	sock.sendall(request)
	
	cdef CString buff = CString (sock.recv(16, 0))
	if func != NULL:
		func (buff.get_c())
	while len(buff) >= 16:
		buff + sock.recv(16, 0) # + op edits the first arg
		if func != NULL:
			func (buff.get_c())
	
	return buff.get_py()