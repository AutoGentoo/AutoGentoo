cdef extern from "<autogentoo/hacksaw/tools/vector.h>":
	cdef:
		ctypedef enum vector_opts:
			VECTOR_UNORDERED = 0x0, # Any order can be used (most efficient)
			VECTOR_ORDERED = 0x1, # Keep the elements in order (don't use if not needed)
			VECTOR_KEEP = 0x2, # Keep the memory unallocated
			VECTOR_REMOVE = 0x0, # Remove the memory and replace the location
	
	cdef:
		ctypedef struct Vector:
			void* ptr; # Location where elements are stored
			size_t n; # Number of filled places
			size_t s; # Max number of elements (increments by HACKSAW_VECTOR_INCREMENT)
			size_t size; # size of each element
		
			size_t increment; # Default is HACKSAW_VECTOR_INCREMENT
		
			vector_opts opts;
	
	cdef:
		Vector* vector_new(size_t el_size, vector_opts opts);
		size_t vector_add(Vector* vec, void* el);
		void vector_remove(Vector* vec, int index);
		void vector_insert(Vector* vec, void* el, int index);
		void vector_extend(Vector* dest, Vector* ex);
		void vector_allocate(Vector* vec);
		void vector_allocate_to_size(Vector* vec, size_t s);
		void** vector_get (Vector* vec, int i);
		void vector_free(Vector* vec);

cdef class CVector:
	cdef Vector* parent;
	cdef int iter_current;
	
	cdef size_t add(self, void* el)
	cdef void remove(self, int index)
	cdef void insert(self, void* el, int index)
	cdef void extend(self, CVector vec)
	cdef void** get(self, int i)