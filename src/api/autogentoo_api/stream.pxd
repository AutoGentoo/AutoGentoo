from .request cimport *

cpdef job_stream(Client client, list args)

cdef class Stream:
	cdef Socket read_from
	
	cdef read_ssl(self)
