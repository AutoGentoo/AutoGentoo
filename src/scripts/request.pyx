cdef extern from "<arpa/inet.h>":
	int htonl (int)
	int ntohl (int)

cdef extern from "<stdlib.h>":
	size_t strlen (char*);

cdef class Request:
	def __init__ (self, request_t rtype, protocol_t prot):
		self.data = DynamicBuffer ()
		self.protocol = prot
		self.type = rtype
		
		cdef int k = 0
		
		if self.protocol == PROT_AUTOGENTOO:
			self.data.append (&k, 1)
			self.add_int(self.type)
	
	cdef add_int (self, int to_add):
		to_add = htonl (to_add)
		self.data.append(&to_add, sizeof (int))
	
	cdef add_hostedit (self, int select_one, int select_two, char* field):
		cdef request_structure_t __type = STRCT_HOSTEDIT
		self.add_int(__type)
		self.add_int(select_one)
		self.add_int(select_two)
		self.data.append_string(field)
	
	cdef add_hostselect (self, char* host_id):
		cdef request_structure_t __type = STRCT_HOSTSELECT
		self.add_int(__type)
		self.data.append_string(host_id)
	
	cdef add_hostinstall (self, char* param):
		cdef request_structure_t __type = STRCT_HOSTINSTALL
		self.add_int(__type)
		self.data.append_string(param)
	
	cdef add_templatecreate (self, char* host_id, char* arch, char* cflags, char* chost, extras=()):
		cdef request_structure_t __type = STRCT_TEMPLATECREATE
		self.add_int(__type)
		self.data.append_string(host_id)
		self.data.append_string(arch)
		self.data.append_string(cflags)
		self.data.append_string(chost)
		
		cdef int extra_c = <int>len(extras)
		self.add_int(extra_c)
		for item in extras:
			self.data.append_string(item.make_extra)
			self.add_int(item.select)
	
	cdef add_hostupload_int (self, hostoffsets_t offset, int ob):
		cdef request_structure_t __type = STRCT_HOSTOFFSET
		self.add_int(__type)
		self.add_int(offset)
		self.add_int(sizeof (int))
		self.add_int(ob)
	
	cdef add_hostupload_str (self, hostoffsets_t offset, char* ob):
		cdef request_structure_t __type = STRCT_HOSTOFFSET
		self.add_int(__type)
		self.add_int(offset)
		self.add_int(strlen(ob) + 1)
		self.data.append_string(ob)
		
	cdef add_hostupload_list (self, hostoffsets_t offset, list ob):
		cdef request_structure_t __type = STRCT_HOSTOFFSET
		self.add_int(__type)
		self.add_int(offset)
		
		cdef size_t k = 0;
		for x in ob:
			k += len(x) + 1
		
		self.add_int(k)
		for x in ob:
			self.data.append_string(x.encode ("utf-8"))
	
	cdef add_templateselect (self, int index):
		cdef request_structure_t __type = STRCT_TEMPLATESELECT
		self.add_int(__type)
		self.add_int(index)
	
	cdef add_stagecommand (self, stage_command_t command):
		cdef request_structure_t __type = STRCT_STAGECOMMAND
		self.add_int(__type)
		self.add_int(command)
	
	cdef finish (self):
		self.add_int(0)
		self.data.print_raw()
		return self.data