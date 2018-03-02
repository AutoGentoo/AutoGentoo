from libcpp cimport bool

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

cimport cython
ctypedef fused primitives:
	cython.char
	cython.int
	

cdef class PyVec:
	cdef Vector* parent
	cdef size_t el_size
	cdef bool is_primitive
	
	def __cinit__(self, size_t el_size, ordered=False, bool is_primitive=False):
		self.parent = vector_new (el_size, <vector_opts>(REMOVE | ORDERED if ordered else UNORDERED))
		self.el_size = el_size
		self.is_primitive = is_primitive
	
	cdef void append (self, void* item):
		if self.is_primitive:
			raise TypeError ("'item' is not a pointer")
		vector_add (self.parent, item)
	
	def __add__ (self, PyVec another):
		vector_extend(<Vector*>self.parent, another.parent)
	
	cdef void remove (self, int index):
		vector_remove(self.parent, index)
	
	cdef void insert (self, void* item, int index):
		if self.is_primitive:
			raise TypeError ("'item' must be a pointer")
		
		vector_insert (self.parent, item, index)
	
	cdef get (self, int index):
		return vector_get (self.parent, index)[0]
	
	def __getitem__ (self, int index):
		return vector_get (self.parent, index)[0]
	
	def __dealloc__ (self):
		vector_free (self.parent)