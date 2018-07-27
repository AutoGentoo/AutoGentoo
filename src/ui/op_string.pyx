from libc.string cimport strlen, strstr

cdef class CString:
	def __cinit__ (self, _in=None, size=32):
		if _in is None:
			self.parent = string_new (size)
		else:
			l = strlen(_in)
			self.parent = string_new (l % 32 + l)
			string_append(self.parent, _in)
	
	cpdef append (self, toappend):
		if isinstance(toappend, CString):
			string_append (self.parent, toappend.parent.ptr)
			return self
		
		
		string_append(self.parent, toappend)
		return self
	
	def __add__ (self, toappend):
		self.append(str(toappend).encode())
	
	def __sub__ (self, CString len_toremove):
		cdef size_t k = strlen (len_toremove.parent.ptr);
		self.parent.n -= k
		self.parent.ptr[self.parent.n] = 0
		return self
	
	def __str__ (self):
		return str(self.parent.ptr)
	
	cpdef find (self, char* _token):
		return CString (strstr (self.parent.ptr, _token))
	
	cdef char* get_c (self):
		return self.parent.ptr
	
	cpdef get_py (self):
		return self.__str__ ()
	
	def __dealloc__ (self):
		string_free (self.parent)
