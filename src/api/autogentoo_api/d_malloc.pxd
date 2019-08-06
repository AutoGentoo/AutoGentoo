from .dynamic_binary cimport *

cdef AUTOGENTOO_FILE_END = 0xffffffff

cdef class DynamicBuffer:
	cdef DynamicBinary* parent

	cdef void append_item (self, str _type, void* data)
	cdef void append_bin (self, void* data, size_t size)
	cdef void append_string (self, char* ptr)
	cdef void append_int (self, int k)
	cpdef append(self, template, args)
	cdef void* get_ptr (self)
	cpdef size_t get_size(self)
	cpdef void print_raw (self, align=*)

cdef class Binary:
	cdef size_t pos
	cdef readonly sentinels
	cdef is_network_endian
	cdef void* ptr
	cdef size_t size
	cdef DynamicBuffer buffer
	
	cdef void set_ptr(self, void* ptr, size_t size)
	cpdef str read_string (self)
	cpdef int read_int (self)
	cdef skip_until (self, to_find)
	cdef inside (self, size_t next_size=*)
	cdef inside_size (self, size_t size=*)
	cpdef add_sentinel (self, int sentinel)
	cdef check_sentinels (self)
	cdef str get_array_template(self, template_start)
	cpdef read_template (self, char* template, top=*)
	cpdef void print_raw (self, align=*)
