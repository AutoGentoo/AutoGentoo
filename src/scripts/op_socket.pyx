from op_socket cimport Address
from socket import AF_INET, SOCK_STREAM

from libc.string cimport memset
from libc.stdlib cimport atoi
from libc.stdint cimport uint16_t

cdef extern from "<arpa/inet.h>":
	uint16_t htons(uint16_t hostshort);
	int inet_pton(int af, const char *src, void *dst);

cdef extern from "<sys/socket.h>":
	ssize_t send(int sockfd, const void *buf, size_t _len, int flags);
	ssize_t recv(int sockfd, void *buf, size_t _len, int flags);
	int socket(int domain, int _type, int protocol);
	int connect(int sockfd, const sockaddr_in *addr, size_t addrlen);

cdef class Socket:
	def __cinit__ (self, Address adr):
		self.adr = adr
		self.fd = socket (AF_INET, SOCK_STREAM, 0)
		memset(&self.address, 0, sizeof (self.address))
		self.address.sin_family = AF_INET
		self.address.sin_port = htons(atoi(adr.port))
		
		if inet_pton(AF_INET, adr.ip, &self.address.sin_addr):
			raise TypeError("Invalid address %s" % adr.ip)
	
	cdef size_t c_send (self, char* data):
		if connect (self.fd, <const sockaddr_in*>&self.address.sin_addr, sizeof (self.address)):
			return -1
		
	
	cdef ssize_t c_recv (self, char* dest, size_t size):
		pass