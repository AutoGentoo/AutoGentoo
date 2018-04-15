from d_malloc cimport DynamicBuffer


cdef class Job:
	cdef DynamicBuffer binary;
	
	