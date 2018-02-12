cdef struct Address:
	char port[4]
	char* ip

cdef class Server:
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
	

cdef char* socket_request (Address adr, char* request):
