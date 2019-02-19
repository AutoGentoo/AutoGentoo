from request cimport *

cdef class Request:
	def __init__(self, request_t req_code, list args):
		self.request = DynamicBuffer(to_network=True) # To big endian
		self.response = DynamicBuffer(is_network=True, to_network=False) # To host endian
		
		self.request.append(request_structure_linkage[req_code - (REQ_START + 1)], args)
		self.req_code = req_code
	
	cpdef size_t send(self, Address adr):
	
	
	cpdef size_t recv(self):
		pass