from libc.stdlib cimport free
from cpython.bytes cimport PyBytes_FromString
from libc.stdio cimport *
from libc.stdlib cimport *
from libc.string cimport *

cdef class PyVec:
	increment = 5
	def __cinit__ (self, ordered=False, free_data=False):
		self.ptr = <void**>malloc (sizeof(void*) * self.increment)
		self.size = 0
		self.vec_size = self.increment
		self.free_data = free_data
	
	cdef void allocate (self):
		self.vec_size += self.increment
		self.ptr = <void**>realloc (self.ptr, sizeof(void*) * self.vec_size)
	
	cdef void append (self, void* item):
		if self.size + 1 >= self.vec_size:
			self.allocate()
		
		self.ptr[self.size] = item
		self.size += 1
	
	cdef remove (self, int index):
		if self.ordered:
			memcpy (&self.ptr[index], &self.ptr[index +1], sizeof(void*) * (self.size - index))
		else:
			self.ptr[index] = self.ptr[self.size - 1]
		self.size -= 1
	
	cdef void* get (self, int index):
		return self.ptr[index]
	
	def __getitem__ (self, index):
		return <object><char*>self.ptr[index]
	
	def __dealloc__ (self):
		if self.free_data:
			for i in range (self.size):
				free (self.get (i))
		free (self.ptr)