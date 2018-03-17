cdef class DynamicBuffer:
	cdef void* ptr
	cdef size_t size
	cdef public size_t n
	cdef short align
	
	cdef realloc (self, size_t size_to_add)
	cdef void append (self, void* ptr, size_t size)

cdef class Binary:
	cdef DynamicBuffer buffer
	cdef size_t pos
	
	cdef char* read_string (self)
	cdef int read_int (self)
	cdef skip_until (self, to_find)
	cdef inside (self)