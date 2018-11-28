from d_malloc cimport DynamicBuffer
from op_socket cimport Address

from libc.stdio cimport sprintf, printf, stdout, fflush
from libc.string cimport strdup
from libc.stdlib cimport atoi, free, malloc
from libc.stdint cimport uint16_t
import ssl, socket

cdef extern from "<arpa/inet.h>":
	uint16_t htons(uint16_t hostshort);
	int inet_pton(int af, const char *src, void *dst);

cdef extern from "<sys/socket.h>":
	ssize_t send(int sockfd, const void *buf, size_t _len, int flags);
	ssize_t recv(int sockfd, void *buf, size_t _len, int flags);
	int connect(int sockfd, const sockaddr_in *addr, size_t addrlen);
	ssize_t write(int fd, const void *buf, size_t count);
	size_t strlen(char*);

from posix.unistd cimport STDOUT_FILENO

cdef class Socket:
	def __init__ (self, Address adr, com_t comm_type = COM_PLAIN):
		self.adr = adr
		self.socket = None
		self.communication_type = comm_type
		self.fd = -1
		self.socket_temp = None
	
	def do_connect(self):
		try:
			self.socket_temp = socket.create_connection((self.adr.ip.decode ("UTF-8"), <object>atoi(self.adr.port)))
		except socket.gaierror:
			raise ConnectionError("Failed to connect")
		
		self.fd = self.socket.fileno()
		if self.communication_type == COM_RSA:
			context = ssl.create_default_context()
			self.socket = context.wrap_socket(self.socket_temp)
		else:
			self.socket = self.socket_temp
	
	cdef c_send (self, void* data, size_t size, do_connect):
		if do_connect:
			self.do_connect()
		return self.socket.send() write (self.fd, data, size)
	
	cdef send (self, DynamicBuffer data, do_connect):
		return self.c_send(data.ptr, data.n, do_connect)
	
	cpdef recv (self, int size):
		cdef char* buffer = <char*>malloc (size)
		cdef size_t out = self.socket.recv_into(buffer, size)
		return out, buffer
	
	cdef size_t recv_into (self, void* buffer, size_t size):
		return recv (self.fd, buffer, size, 0)
	
	cpdef close (self):
		return self.socket.close ()
	
	cpdef DynamicBuffer request (self, DynamicBuffer request, _print=False, _print_raw=False, _store=True):
		cdef char* c_req;
		
		self.send (request, True)
		
		cdef DynamicBuffer out_data_raw = DynamicBuffer (size=128)
		
		cdef void* buffer = malloc (132)
		cdef size_t size = self.recv_into(buffer, 128)
		while size > 0:
			if _print_raw:
				print_raw (<char*>buffer, size)
			if _print:
				write (STDOUT_FILENO, buffer, size)
			if _store:
				out_data_raw.append (buffer, size)
			else:
				out_data_raw.set (buffer, 0, size)
			size = self.recv_into(buffer, 128)
		
		free (buffer)
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

cdef print_raw (void* ptr, size_t n, int last_i = 1, align=True):
	for i in range (n):
		printf ("%02x ", (<char*>ptr)[i] & 0xff)
		if last_i % 25 == 0 and align:
			printf ("\n")
		last_i += 1
	fflush (stdout)
	return last_i
