from cython_dynamic_binary cimport *

cdef AUTOGENTOO_FILE_END = 0xffffffff

cdef class DynamicBuffer:
	cdef DynamicBinary* parent

	cdef void append_item (self, char _type, void* data)
	cdef void append_bin (self, void* data, size_t size)
	cdef void append_string (self, char* ptr)
	cdef void append_int (self, int k)
	cpdef append(self, str template, list args)
	cdef void* get_ptr (self)
	cpdef size_t get_size(self)
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
