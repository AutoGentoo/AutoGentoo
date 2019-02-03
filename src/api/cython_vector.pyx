from cython_vector cimport *

cdef class CVector:
	def __init__(self, element_size, keep=True, ordered=True):
		self.iter_current = 0
		cdef int opts = 0;
		opts |= VECTOR_KEEP if keep else VECTOR_REMOVE
		opts |= VECTOR_ORDERED if ordered else VECTOR_UNORDERED
		self.parent = vector_new(element_size, <vector_opts>opts)
	
	cdef size_t add(self, void* el):
		return vector_add(self.parent, el)
	
	cdef void remove(self, int index):
		vector_remove(self.parent, index)
	
	cdef void insert(self, void* el, int index):
		vector_insert(self.parent, el, index)
	
	cdef void extend(self, CVector vec):
		vector_extend(self.parent, vec.parent)
	
	cdef void** get(self, int i):
		return vector_get(self.parent, i)
	
	def __len__(self):
		return self.parent.n
	
	def __dealloc__(self):
		vector_free(self.parent)