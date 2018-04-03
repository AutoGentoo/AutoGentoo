from d_malloc cimport DynamicBuffer

enum Jobs:


cdef class Job:
	cdef DynamicBuffer binary;
	
	