from job cimport *
from request cimport *

cdef class Worker:
	cdef Server server
	cdef queue
	
	cpdef flush_queue(self)
	cdef parse_queue(self)