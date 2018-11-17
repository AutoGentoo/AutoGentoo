from libc.stdio cimport printf, fflush, stdout
from libc.stdlib cimport malloc, realloc, free
from libc.string cimport strlen, strdup, memcpy
from interface cimport AUTOGENTOO_FILE_END

cdef extern from "<arpa/inet.h>":
	int ntohl (int)
	int htonl (int)

cdef class DynamicBuffer:
	@classmethod
	cdef new_from_initial(void* initial, size_t size):
		self = DynamicBuffer()
		free(self.ptr)
		self.ptr = malloc(size)
		self.size = size
		
		self.append(initial, size)
		return self
	
	def __init__ (self, char* start=<char*>NULL, size_t size=32, short align=32):
		if size > 0:
			self.ptr = malloc (size)
			self.size = size
		else:
			self.ptr = malloc (32)
			self.size = 32
		
		self.n = 0
		self.align = align
		
		if start and start is not None:
			self.append (<char*>start, strlen(<char*>start))
	
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
	
	cdef void* get_ptr (self):
		return self.ptr
	
	def __dealloc__ (self):
		if self.ptr != NULL:
			free (self.ptr)
			self.ptr = NULL
		
	
	cpdef void print_raw (self, align=25):
		cdef int last_i = 1
		
		for i in range (self.n):
			printf ("%02x ", (<char*>self.ptr)[i] & 0xff)
			if last_i % 25 == 0 and align:
				printf ("\n")
			last_i += 1
		fflush (stdout)

cdef class Binary:
	def __init__ (self, DynamicBuffer buffer):
		self.buffer = buffer
		self.ptr = buffer.get_ptr ()
		
		self.pos = 0
		self.sentinels = []
		
	cdef char* read_string (self):
		if not self.inside():
			return NULL
		
		cdef char* out = <char*>(<void*>self.ptr + self.pos)
		self.pos += strlen (out) + 1
		
		return strdup (out)
	
	cdef int read_int (self):
		if not self.inside(sizeof (int)):
			return 0
		
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
		
		while c not in to_find and c != eof_be and self.pos < self.buffer.n:
			memcpy (&c, self.ptr + self.pos, sizeof (int))
			self.pos += 1
		self.pos -= 1
		
		return self.pos + 1 < self.buffer.n
	
	cpdef add_sentinel (self, int sentinel):
		self.sentinels.append (sentinel)
	
	cdef inside_size (self, size_t size=0):
		return self.pos < (self.buffer.n + size)
	
	cdef check_sentinels (self):
		if len(self.sentinels) == 0:
			return False
		
		if not self.inside_size(sizeof (int)):
			return True
		
		cdef int out
		memcpy (&out, self.ptr + self.pos, sizeof (int))
		
		for sentinel in self.sentinels:
			if out == sentinel:
				return True
		return False
	
	cdef inside (self, size_t next_size=0):
		return not self.check_sentinels() and self.inside_size(next_size)
	
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
