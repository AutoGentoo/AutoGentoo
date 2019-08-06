from .dynamic_binary cimport DynamicType
from .d_malloc cimport DynamicBuffer

cdef extern from "<autogentoo/request_structure.h>":
	ctypedef enum request_structure_t:
		STRCT_END,
		STRCT_HOST_NEW = 1,
		STRCT_HOST_SELECT,
		STRCT_HOST_EDIT,
		STRCT_AUTHORIZE,
		STRCT_EMERGE,
		STRCT_ISSUE_TOK,
		STRCT_JOB_SELECT,
		
		STRCT_MAX

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
			REQ_SRV_REFRESH,
			REQ_AUTH_ISSUE_TOK,
			REQ_AUTH_REFRESH_TOK,
			REQ_AUTH_REGISTER,
			REQ_JOB_STREAM,
			
			REQ_MAX

cdef extern from "<autogentoo/api/request_generate.h>":
	ctypedef struct ClientRequest:
		# We dont use these
		request_t request_type;
		void* arguments;
		
		# /* Just set the dynamic binary ptr */
		size_t size;
		void* ptr;
	
	cdef:
		ClientRequest* client_request_init(request_t _type);
		int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, DynamicType* content);
		int client_request_generate(ClientRequest* req);
		void client_request_free(ClientRequest* req);

cdef extern from "<autogentoo/api/ssl_wrap.h>":
	cdef:
		ctypedef struct SSocket:
			void* ssl;
			void* cert;
			void* cert_name;
			void* context;
			
			char* hostname;
			unsigned short port;
			int socket;
		
		int ssocket_new(SSocket** sock, char* server_hostname, unsigned short port);
		void ssocket_free(SSocket* ptr);
		void autogentoo_client_ssl_init();
		void ssocket_request(SSocket* ptr, ClientRequest* request);
		void socket_request(int sock, ClientRequest* request);
		int ssocket_read(SSocket* ptr, void* dest, int is_server);
		int socket_read(int ptr, void* dest, int is_server);
		int socket_connect(char* hostname, unsigned short port); # Not actually in the header (private)

cdef extern from "<autogentoo/user.h>":
	ctypedef enum token_access_t:
		TOKEN_NONE,
		TOKEN_SERVER_READ = 1 << 0,
		TOKEN_SERVER_WRITE = TOKEN_SERVER_READ | 1 << 1, # //!< Create hosts
		TOKEN_SERVER_AUTOGENTOO_ORG = 1 << 2, # //!< Register users from server (no read/write)
		TOKEN_HOST_READ = 1 << 3,
		TOKEN_HOST_EMERGE = TOKEN_HOST_READ | 1 << 4, # //!< Can't change host settings
		TOKEN_HOST_WRITE = TOKEN_HOST_EMERGE | 1 << 5, # //!< Write to make.conf
		TOKEN_HOST_MOD = TOKEN_HOST_WRITE | 1 << 6, # //!< Can delete host
		TOKEN_SERVER_SUPER = 0xFF, # //!< All permissions

cdef extern from "<sys/un.h>":
	ctypedef unsigned short int sa_family_t;
	
	cdef struct sockaddr_un:
		sa_family_t sun_family;
		char sun_path[108];

cdef extern from "<sys/socket.h>":
	cdef enum:
		AF_UNIX=1
		SOCK_STREAM = 1
	
	ctypedef unsigned int socklen_t;
	cdef struct sockaddr:
		sa_family_t sa_family;
		char sa_path[14];
	
	int connect (int __fd, const sockaddr * __addr, socklen_t __len);
	int socket (int __domain, int __type, int __protocol);

cdef extern from "<openssl/ssl.h>":
	ctypedef struct SSL:
		pass
	
	int SSL_read(SSL *ssl, void *buf, int num);
	int SSL_write(SSL *ssl, void *buf, int num);

ctypedef sockaddr __typ_sockaddr
ctypedef sockaddr_un __typ_sockaddr_un

cdef extern from "<Python.h>":
	bytearray PyByteArray_FromStringAndSize(char *string, Py_ssize_t len)

cdef class Socket:
	cdef SSocket* secure_socket
	cdef int raw_socket
	cdef ssl
	cdef Address adr
	
	cdef c_send(self, void* buffer, int size)
	cdef c_recv(self, void* buffer, int size)
	cpdef request(self, DynamicBuffer request)
	cpdef raw_send(self, DynamicBuffer ptr)
	cpdef recv(self, raw=*)

cdef class Request:
	cdef Socket sock
	cdef DynamicBuffer request
	
	cdef int code
	cdef str message
	cdef error
	
	cpdef size_t send(self)
	cpdef list recv(self, raw=*)

cdef class Address:
	cdef char* ip
	cdef int port
	cdef unix_socket

cdef class Client:
	cdef Address adr
	
	cpdef request(self, str str_code, args)
	cdef verify_request(self, request_t code, args)

cpdef host_new(str arch, str profile, str hostname)
# request_type 1: make_conf 2: general
cpdef host_edit(int request_type, str make_conf_var, str make_conf_val)
cpdef host_select(str hostname)
cpdef authorize(str user_id, str token)
cpdef host_emerge(str emerge_str)
cpdef issue_token(str user_id, str target_host, token_access_t access_level)
cpdef job_select(str job_id)
