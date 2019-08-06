from .stream cimport *

cpdef job_stream(Client client, list args):
	client.verify_request(<request_t>REQ_JOB_STREAM , args)
	cdef Request req = Request(client.adr, <request_t>REQ_JOB_STREAM , args)
	if req.error:
		return None

	req.send()
	return Stream(req.sock)


cdef class Stream:
	def __init__(self, Socket read_from):
		self.read_from = read_from
	
	def __next__(self):
		return self.read_ssl()
	
	def __iter__(self):
		return self
	
	cdef read_ssl(self):
		if self.read_from is None:
			raise StopIteration()
		
		cdef int chunk_size = 32;
		cdef char buff[32];
		cdef ssize_t read_size = SSL_read(<SSL*>self.read_from.secure_socket.ssl, buff, chunk_size)
		
		if read_size <= 0:
			raise StopIteration()
		
		return buff[0:read_size]
