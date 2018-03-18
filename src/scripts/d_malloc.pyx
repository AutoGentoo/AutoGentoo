from libc.stdlib cimport malloc, realloc, free
from libc.string cimport strlen, strdup, memcpy
from interface cimport AUTOGENTOO_FILE_END

cdef extern from "<arpa/inet.h>":
	int ntohl (int)
	int htonl (int)

cdef class DynamicBuffer:
	def __init__ (self, char* start=NULL, size_t size=32, short align=32):
		self.ptr = malloc (size)
		self.size = size
		self.n = 0
		self.align = align
		
		if start:
			self.append (start, strlen(start))
	
	cdef realloc (self, size_t size_to_add):
		self.size += size_to_add
		self.ptr = realloc (self.ptr, self.size)
	
	cdef void append (self, void* ptr, size_t size):
		if size == 0 or ptr == NULL:
			return
		
		if self.n + size >= self.size:
			final_unaligned = self.size + size
			self.realloc(size + (final_unaligned % self.align))
		
		memcpy (self.ptr + self.n, ptr, size)
		self.n += size
	
	cdef void set (self, void* ptr, int start, size_t size):
		if size == 0 or ptr == NULL:
			return
		
		if start + size >= self.size:
			final_unaligned = start + size
			self.realloc(size + (final_unaligned % self.align))
		
		memcpy (self.ptr + start, ptr, size)
		
		if start + size >= self.n:
			self.n = start + size
	
	def __dealloc__ (self):
		free (self.ptr)

cdef class Binary:
	def __init__ (self, DynamicBuffer buffer):
		self.buffer = buffer
		self.pos = 0
	
	cdef char* read_string (self):
		cdef char* out = <char*>(<void*>self.buffer.ptr + self.pos)
		self.pos += strlen (out) + 1
		
		return strdup (out)
	
	cdef int read_int (self):
		cdef int out
		memcpy (&out, self.buffer.ptr + self.pos, sizeof (int))
		self.pos += sizeof (int)
		return ntohl(out)
	
	cdef skip_until (self, to_find):
		if sizeof(to_find) == 1:
			to_find = [htonl (to_find)]
		else:
			to_find = [htonl(x) for x in to_find]
		
		cdef int eof_be = htonl (AUTOGENTOO_FILE_END)
		cdef int c
		
		while c not in to_find and c != eof_be and self.pos < self.buffer.n:
			memcpy (&c, self.buffer.ptr + self.pos, sizeof (int))
			
			self.pos += 1
		self.pos -= 1
		
		return self.pos + 1 < self.buffer.n
	
	cdef inside (self):
		return self.pos < self.buffer.n