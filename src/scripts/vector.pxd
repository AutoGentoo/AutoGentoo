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
	Vector* vector_new(size_t el_size, vector_opts opts)
	void vector_extend(Vector* dest, Vector* ex);
	void vector_free(Vector* vec);
	size_t vector_add(Vector* vec, void* el);
	void vector_remove(Vector* vec, int index);
	void vector_insert(Vector* vec, void* el, int index);
	char* vector_get(Vector* vec, int i);
	void vector_free(Vector* vec);

cdef class PyVec:
	cdef void** ptr
	cdef size_t size # number of items in vec
	cdef size_t vec_size # number of slots
	
	cdef bool ordered
	cdef bool free_data
	
	cdef void append (self, void* item)
	cdef void allocate (self)
	cdef remove (self, int index)
	cdef void* get (self, int index)
