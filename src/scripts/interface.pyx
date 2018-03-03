from libc.stdlib cimport atoi
from libc.stdio cimport printf

import socket

from op_string cimport CString
from vector cimport PyVec
from op_socket cimport Address, Socket

cdef class Host
cdef class Stage

cdef class Server:
	cdef Address adr
	cdef char* target;
	
	cdef PyVec hosts
	cdef dict staged;
	
	def __cinit__ (self, Address adr):
		self.hosts = PyVec ()
		self.stages = {}
		self.adr = adr
	
	cdef void read_server (self):
		cdef Socket t_sock = Socket (self.adr)
		

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

cdef socket_request (Address adr, char* request, _print=False):
	cdef int portno = atoi (adr.port);
	sock = socket (socket.AF_INET, socket.SOCK_STREAM)
	
	sock.connect ((adr.ip.decode("UTF-8"), portno))
	sock.sendall(request)
	
	pb = sock.recv(16, 0)
	cdef CString buff = CString (<char*>pb)
	cdef char* k
	if _print:
		printf ("%s", buff.get_c())
	while len(buff) >= 16:
		pb = sock.recv(16, 0)
		if _print:
			printf ("%s", <char*>pb)
		buff + <char*>pb
	
	return buff.get_py()