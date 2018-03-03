from op_socket cimport Address
from op_string cimport CString
from socket import AF_INET, SOCK_STREAM, socket

from posix.unistd cimport close
from libc.errno cimport errno
from libc.stdio cimport printf, sprintf
from libc.string cimport memset, strlen, strdup
from libc.stdlib cimport atoi, malloc, free
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
		self.socket = socket (AF_INET, SOCK_STREAM, 0)
		self.fd = self.socket.fileno()
		if self.fd < 0:
			raise IOError ("Failed to create socket")
	
	cdef size_t c_send (self, char* data, do_connect=True):
		if do_connect:
			if self.socket.connect ((self.adr.ip.decode ("UTF-8"), <object>atoi(self.adr.port))):
				raise ConnectionError("Failed to connect")
		cdef ssize_t out = send (self.fd, data, strlen(data), 0)
		if out == -1:
			raise IOError ("Failed to send errno: '%d'" % <int>errno)
		
		return out
	
	cdef ssize_t c_recv (self, char* dest, size_t size):
		return recv (self.fd, dest, size, 0)
	
	def send (self, object data, do_connect=True):
		return <object>self.c_send (str(data).encode ("UTF-8"), do_connect)
	
	def recv (self, object size, do_connect=True):
		cdef char* data = <char*>malloc (size % 32 + size)
		cdef size_t read_bytes = self.c_recv(data, size)
		cdef object out = str(data.decode("UTF-8"))
		
		return <object> (out, <object>read_bytes)
	
	cpdef int close (self):
		return close (self.fd)
	
	cpdef CString request (self, request, _print=False):
		cdef char* c_req;
		
		if isinstance(request, unicode):
			c_req = strdup (request.encode ("UTF-8"))
		else:
			c_req = strdup(<char*>request)
		self.c_send (c_req, True)
		cdef CString out_data = CString (size=32)
		
		cdef char* buffer = <char*>malloc (20)
		buffer[0] = 0
		while self.c_recv(buffer, 16) > 0:
			if _print:
				printf ("%s", buffer)
			out_data.append (buffer)
			buffer[0] = 0
		
		free (buffer)
		free (c_req)
		
		return out_data
	
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