from job cimport *
from request cimport *

cdef class Worker:
	cdef Server server
	cdef queue
	
	cdef parse_queue(self)