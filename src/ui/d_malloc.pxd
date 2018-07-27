cdef class DynamicBuffer:
	cdef void* ptr
	cdef size_t size
	cdef public size_t n
	cdef short align
	
	cdef realloc (self)
	cdef void append (self, void* ptr, size_t size)
	cdef void append_string (self, char* ptr)
	cdef void append_int (self, int k)
	cdef void set (self, void* ptr, int start, size_t size)
	cdef void* get_ptr (self)
	cpdef void print_raw (self, align=*)

cdef class Binary:
	cdef size_t pos
	cdef readonly sentinels
	cdef void* ptr
	cdef DynamicBuffer buffer
	
	cdef char* read_string (self)
	cdef int read_int (self)
	cdef skip_until (self, to_find)
	cdef inside (self, size_t next_size=*)
	cdef inside_size (self, size_t size=*)
	cpdef add_sentinel (self, int sentinel)
	cdef check_sentinels (self)
	cpdef read_template (self, char* template)