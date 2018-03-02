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
	
	def __cinit__ (self, _in=None):
		if _in is None:
			self.parent = string_new (32)
		else:
			l = strlen(_in)
			self.parent = string_new (l % 32 + l)
			string_append(self.parent, _in)
	
	def append (self, char* toappend):
		string_append(self.parent, toappend)
		return self
	
	def append (self, CString toappend):
		string_append (self.parent, toappend.parent.ptr)
		return self
	
	def __add__ (self, toappend):
		self.append(str(toappend).encode())
	
	def __sub__ (self, CString len_toremove):
		cdef size_t k = strlen (len_toremove.parent.ptr);
		self.parent.n -= k
		self.parent.ptr[self.parent.n] = 0
		return self
	
	def __str__ (self):
		return self.parent.ptr.decode ("UTF-8")
	
	cdef find (self, char* _token):
		return CString (strstr (self.parent.ptr, _token))
	
	cdef get_c (self):
		return self.parent.ptr
	
	cdef get_py (self):
		return self.__str__ ()
	
	def __dealloc__ (self):
		string_free (self.parent)

def cstr_to_str (String s):
	return s.ptr.decode ("UTF-8")