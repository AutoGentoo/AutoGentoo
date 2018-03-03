from libc.string cimport strlen, strstr

cdef extern from "<autogentoo/hacksaw/tools/string.h>":
	cdef struct __String:
		char* ptr;
		size_t n;
		size_t size;
		int increment;
	ctypedef __String String
	
	String* string_new(size_t start);
	void string_append(String* dest, char*);
	void string_append_c(String* dest, int c);
	void string_allocate(String* string);
	int string_find(char** array, char* element, size_t n);
	void string_free(String* string);
	void fix_path(char* ptr);
	void string_overwrite(char** dest, char* src, int dup);
	char* string_join(char** src, char* delim, int n);

cdef class CString:
	cdef String* parent
	
	cpdef append (self, char* toappend)
	cpdef find (self, char* _token)
	cdef char* get_c (self)
	cpdef get_py (self)