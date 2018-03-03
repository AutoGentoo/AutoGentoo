cdef class PyVec:
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
	
	def __getitem__ (self, index):
		return <object>self.get(<int>index)
	
	def __dealloc__ (self):
		vector_free (self.parent)