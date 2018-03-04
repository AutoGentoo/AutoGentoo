from socket import AF_INET, SOCK_STREAM, socket
from d_malloc cimport DynamicBuffer
from op_socket cimport Address

from libc.stdio cimport sprintf, printf, stdout, fflush
from libc.string cimport strdup
from libc.stdlib cimport atoi, free, malloc
from libc.stdint cimport uint16_t


cdef extern from "<arpa/inet.h>":
	uint16_t htons(uint16_t hostshort);
	int inet_pton(int af, const char *src, void *dst);

cdef extern from "<sys/socket.h>":
	ssize_t send(int sockfd, const void *buf, size_t _len, int flags);
	ssize_t recv(int sockfd, void *buf, size_t _len, int flags);
	int connect(int sockfd, const sockaddr_in *addr, size_t addrlen);

cdef class Socket:
	def __init__ (self, Address adr):
		self.adr = adr
		self.socket = None
		self.fd = -1
	
	cpdef send (self, object data, do_connect=True):
		if do_connect:
			self.socket = socket (AF_INET, SOCK_STREAM, 0)
			self.fd = self.socket.fileno()
			if self.fd < 0:
				raise IOError ("Failed to create socket")
			if self.socket.connect ((self.adr.ip.decode ("UTF-8"), <object>atoi(self.adr.port))):
				raise ConnectionError("Failed to connect")
		return self.socket.send (data)
	
	cpdef recv (self, int size):
		cdef char* buffer = <char*>malloc (size)
		cdef size_t out = self.socket.recv_into(buffer, size)
		return out, buffer
	
	cdef size_t recv_into (self, void* buffer, size_t size):
		return recv (self.fd, buffer, size, 0)
	
	cpdef close (self):
		return self.socket.close ()
	
	cpdef DynamicBuffer request (self, request, _print=False, _print_raw=False):
		cdef char* c_req;
		
		if isinstance(request, unicode):
			c_req = strdup ((<unicode>request).encode ("UTF-8"))
		else:
			c_req = strdup(<char*>request)
		self.send (c_req, True)
		
		cdef DynamicBuffer out_data_raw = DynamicBuffer (size=128)
		
		cdef void* buffer = malloc (128)
		cdef size_t size = self.recv_into(buffer, 128)
		while size > 0:
			if _print_raw:
				print_raw (<char*>buffer, size)
			if _print:
				printf ("%s", <char*>buffer)
			out_data_raw.append (buffer, size)
			size = self.recv_into(buffer, 128)
		
		free (buffer)
		free (c_req)
		self.close ()
		
		return out_data_raw
	
	def __dealloc__ (self):
		pass

cdef class Address:
	def __init__ (self, ip, port):
		if isinstance(ip, unicode):
			self.ip = strdup(ip.encode ("UTF-8"))
		else:
			self.ip = strdup (ip)
		sprintf (self.port, "%d", <int>port)
	
	def __dealloc__ (self):
		free (self.ip)

cpdef print_raw (char* ptr, size_t n):
	for i in range (n):
		printf ("%02X ", ptr[i])
	fflush (stdout)