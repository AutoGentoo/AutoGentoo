from op_string cimport CString
from d_malloc cimport DynamicBuffer

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
	
	cpdef send (self, object data, do_connect=*)
	cpdef recv (self, int size)
	cdef size_t recv_into (self, void* buffer, size_t size)
	
	@staticmethod
	cdef print_raw (char* ptr, size_t n)
	
	cpdef DynamicBuffer request_raw (self, request, _print=*)
	cpdef CString request (self, request, _print=*)
	
	cpdef close (self)
