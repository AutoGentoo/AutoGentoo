from libc.stdio cimport printf, fflush, stdout
from libc.stdlib cimport malloc, realloc, free
from libc.string cimport strlen, strdup, memcpy
from interface cimport AUTOGENTOO_FILE_END

cdef extern from "<arpa/inet.h>":
	int ntohl (int)
	int htonl (int)

cdef class DynamicBuffer:
	def __init__ (self, char* start=NULL, size_t size=32, short align=32):
		if size != 0:
			self.ptr = malloc (size)
		
		self.size = size
		self.n = 0
		self.align = align
		
		if start or start is not None:
			self.append (start, strlen(start))
	
	cdef realloc (self):
		self.size *= 2
		self.ptr = realloc (self.ptr, self.size)
	
	cdef void append_string (self, char* ptr):
		self.append (ptr, strlen(ptr) + 1)
	
	cdef void append_int (self, int k):
		self.append (&k, sizeof (int))
	
	cdef void append (self, void* ptr, size_t size):
		if size == 0 or ptr == NULL:
			return
		
		while self.n + size >= self.size:
			self.realloc()
		
		memcpy (self.ptr + self.n, ptr, size)
		self.n += size
	
	cdef void set (self, void* ptr, int start, size_t size):
		if size == 0 or ptr == NULL:
			return
		
		while start + size >= self.size:
			self.realloc()
		
		memcpy (self.ptr + start, ptr, size)
		
		self.n = start + size
	
	def __dealloc__ (self):
		free (self.ptr)
	
	cpdef void print_raw (self, align=True):
		align = 25
		cdef int last_i = 1
		
		for i in range (self.n):
			printf ("%02x ", (<char*>self.ptr)[i] & 0xff)
			if last_i % 25 == 0 and align:
				printf ("\n")
			last_i += 1
		fflush (stdout)

cdef class Binary(DynamicBuffer):
	def __init__ (self, DynamicBuffer buffer):
		super (Binary, self).__init__ (None, 0, 32)
		
		self.size = buffer.size
		self.ptr = buffer.ptr
		self.pos = 0
	
	
	cdef char* read_string (self):
		cdef char* out = <char*>(<void*>self.ptr + self.pos)
		self.pos += strlen (out) + 1
		
		return strdup (out)
	
	cdef int read_int (self):
		cdef int out
		memcpy (&out, self.ptr + self.pos, sizeof (int))
		self.pos += sizeof (int)
		return ntohl(out)
	
	cdef skip_until (self, to_find):
		if sizeof(to_find) == 1:
			to_find = [htonl (to_find)]
		else:
			to_find = [htonl(x) for x in to_find]
		
		cdef int eof_be = htonl (AUTOGENTOO_FILE_END)
		cdef int c
		
		while c not in to_find and c != eof_be and self.pos < self.n:
			memcpy (&c, self.ptr + self.pos, sizeof (int))
			
			self.pos += 1
		self.pos -= 1
		
		return self.pos + 1 < self.n
	
	cdef inside (self):
		return self.pos < self.n
	
	cpdef read_template (self, char* template):
		out = []
		cdef char* i = template
		while i != NULL:
			if i[0] == 'i':
				out.append (self.read_int())
			elif i[0] == 's':
				out.append (self.read_string())
			
			i += 1
		return out
