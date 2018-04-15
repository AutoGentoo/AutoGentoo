cdef extern from "<arpa/inet.h>":
	int htonl (int)

cdef class Request:
	def __init__ (self, request_t rtype, protocol_t prot):
		self.data = DynamicBuffer ()
		self.protocol = prot
		self.request_type = rtype
		
		cdef int k = 0
		
		if self.protocol == PROT_AUTOGENTOO:
			
			self.data.append (&k, 1)
			self.add_int(self.request_type)
			self.add_int(k) # Start the count at zero
			
			self.count_ptr = <int*>self.data.ptr + self.data.n - sizeof (int)
	
	cdef add_int (self, int to_add):
		to_add = htonl (to_add)
		self.data.append(&to_add, sizeof (int))
	
	cdef add_hostedit (self, int select_one, int select_two, char* field):
		cdef request_structure_t __type = STRCT_HOSTEDIT
		self.add_int(__type)
		self.add_int(select_one)
		self.add_int(select_two)
		self.data.append_string(field)
		
		self.increment()
	
	cdef add_hostselect (self, char* host_id):
		cdef request_structure_t __type = STRCT_HOSTSELECT
		self.add_int(__type)
		self.data.append_string(host_id)
		
		self.increment()
	
	cdef add_hostinstall (self, char* param):
		cdef request_structure_t __type = STRCT_HOSTINSTALL
		self.add_int(__type)
		self.data.append_string(param)
		
		self.increment()
	
	cdef add_templatecreate (self, char* host_id, char* arch, char* cflags, char* chost, extras=()):
		cdef request_structure_t __type = STRCT_TEMPLATECREATE
		self.add_int(__type)
		self.data.append_string(host_id)
		self.data.append_string(arch)
		self.data.append_string(cflags)
		self.data.append_string(chost)
		
		self.increment()
		
		cdef int extra_c = <int>len(extras)
		self.add_int(extra_c)
		for item in extras:
			self.data.append_string(item.make_extra)
			self.add_int(item.select)
		
		self.increment()
	
	cdef add_stageselect (self, int index):
		cdef request_structure_t __type = STRCT_STAGESELECT
		self.add_int(__type)
		self.add_int(index)
		
		self.increment()
	
	cdef add_stagecommand (self, stage_command_t command):
		cdef request_structure_t __type = STRCT_STAGECOMMAND
		self.add_int(__type)
		self.add_int(command)
		
		self.increment()
	
	cdef increment (self):
		self.count_ptr[0] = 1