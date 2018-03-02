from libcpp cimport bool
from libc.stdio cimport printf

cdef extern from "<autogentoo/hacksaw/tools/vector.h>":
	cdef enum __vector_opts:
		UNORDERED = 0x0, # Any order can be used (most efficient)
		ORDERED = 0x1, # Keep the elements in order (don't use if not needed)
		KEEP = 0x2, # Keep the memory unallocated
		REMOVE = 0x0, # Remove the memory and replace the location
	
	struct __Vector:
		void* ptr; # Location where elements are stored
		size_t n; # Number of filled places
		size_t s; # Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)
		size_t size; # size of each element
	
		size_t increment; # Default is HACKSAW_VECTOR_INCREMENT
	
		bool ordered;
		bool keep;
		
	ctypedef __vector_opts vector_opts
	ctypedef __Vector Vector
	
	Vector* vector_new(size_t el_size, vector_opts opts);
	size_t vector_add(Vector* vec, void* el);
	void vector_remove(Vector* vec, int index);
	void vector_insert(Vector* vec, void* el, int index);
	void vector_extend(Vector* dest, Vector* ex);
	void vector_allocate(Vector* vec);
	void vector_allocate_to_size(Vector* vec, size_t s);
	void* vector_get(Vector* vec, int i);
	void vector_free(Vector* vec);

cimport cpython
ctypedef fused primitives:
	char
	int
	size_t
	short
	long
	double
	float

cdef class PyVec:
	cdef Vector* parent
	cdef size_t el_size
	
	def __init__(self, size_t el_size=sizeof (void*), ordered=False):
		self.parent = vector_new (el_size, <vector_opts>(REMOVE | ORDERED if ordered else UNORDERED))
		self.el_size = el_size
	
	cpdef append (self, item):
		cdef void* t = <void*> item;
		vector_add (self.parent, &t)
	
	def __add__ (self, PyVec another):
		vector_extend(<Vector*>self.parent, another.parent)
	
	cpdef void remove (self, int index):
		vector_remove(self.parent, index)
	
	cpdef void insert (self, item, int index):
		cdef void* k = <void*>item;
		vector_insert (self.parent, &k, index)
	
	cdef void* get (self, int index):
		return (<void**>vector_get (self.parent, index))[0]
	
	def get (self, int index):
		return <object>((<void**>vector_get (self.parent, index))[0])
	
	def __getitem__ (self, index):
		return self.get(<int>index)
	
	def __dealloc__ (self):
		vector_free (self.parent)