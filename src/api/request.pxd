from d_malloc cimport DynamicBuffer

cdef extern from "<autogentoo/request.h>":
	cdef:
		ctypedef enum request_t:
			REQ_GET,
			REQ_HEAD,
			REQ_POST,
			REQ_START,
			
			REQ_HOST_NEW,
			REQ_HOST_EDIT,
			REQ_HOST_DEL,
			REQ_HOST_EMERGE,
			
			REQ_HOST_MNTCHROOT,
			REQ_SRV_INFO,
			REQ_AUTH_ISSUE_TOK,
			
			REQ_MAX

cdef char** request_structure_linkage = [
	"sss",# /* Host new */
	"s", # /* Host select */
	"iss", # /* Host edit */
	"ss", # /* Host authorize */
	"s", # /* Emerge arguments */
	"ssi", # /* Issue Token */
]

cdef class Address:
	cdef char port[4]
	cdef char* ip

cdef class Request:
	cdef DynamicBuffer request;
	cdef DynamicBuffer response;
	
	cdef request_t req_code
	
	cpdef size_t send(self, Address adr)
	cpdef size_t recv(self)

# Generates requests and parses responses
cdef class Client:
	cdef Address adr