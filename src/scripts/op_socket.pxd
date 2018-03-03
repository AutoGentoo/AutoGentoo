from op_string cimport CString

cdef extern from "<netinet/ip.h>":
	ctypedef unsigned short int sa_family_t
	ctypedef unsigned short int in_port_t
	ctypedef unsigned int in_addr_t;
	
	struct sockaddr_in:
		sa_family_t sin_family
		in_port_t sin_port;
		in_addr_t sin_addr;

cdef class Address:
	cdef char port[4]
	cdef char* ip
	

cdef class Socket:
	cdef socket
	cdef Address adr
	cdef sockaddr_in address
	cdef int fd
	
	cdef size_t c_send (self, char* data, do_connect=*)
	cdef ssize_t c_recv (self, char* dest, size_t size)
	cpdef CString request (self, request, _print=*)
	cpdef int close (self)
