from op_socket cimport Address
from op_string cimport CString
from socket import AF_INET, SOCK_STREAM, socket

from posix.unistd cimport close
from libc.stdio cimport sprintf
from libc.string cimport strdup
from libc.stdlib cimport atoi, free
import sys
from libc.stdint cimport uint16_t

cdef extern from "<arpa/inet.h>":
	uint16_t htons(uint16_t hostshort);
	int inet_pton(int af, const char *src, void *dst);

cdef extern from "<sys/socket.h>":
	ssize_t send(int sockfd, const vogit id *buf, size_t _len, int flags);
	ssize_t recv(int sockfd, void *buf, size_t _len, int flags);
	int connect(int sockfd, const sockaddr_in *addr, size_t addrlen);

cdef class Socket:
	def __init__ (self, Address adr):
		self.adr = adr
		self.socket = socket (AF_INET, SOCK_STREAM, 0)
		self.fd = self.socket.fileno()
		if self.fd < 0:
			raise IOError ("Failed to create socket")
	
	cpdef send (self, object data, do_connect=True):
		if do_connect:
			if self.socket.connect ((self.adr.ip.decode ("UTF-8"), <object>atoi(self.adr.port))):
				raise ConnectionError("Failed to connect")
		return self.socket.send (data)
	
	cpdef recv (self, int size, do_connect=True):
		return self.socket.recv(size)
	
	cpdef int close (self):
		return close (self.fd)
	
	cpdef CString request (self, request, _print=False):
		cdef char* c_req;
		
		if isinstance(request, unicode):
			c_req = strdup (request.encode ("UTF-8"))
		else:
			c_req = strdup(<char*>request)
		self.send (c_req, True)
		cdef CString out_data = CString (size=128)
		
		buffer = self.recv(128)
		while len(buffer) > 0:
			if _print:
				sys.stdout.write ("%s", buffer)
			out_data.append (buffer)
			buffer = self.recv(128)
		
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