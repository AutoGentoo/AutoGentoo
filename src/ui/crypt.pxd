from bignum cimport BIGNUM
from d_malloc cimport DynamicBuffer
from op_socket cimport Socket

cdef extern from "<openssl/rsa.h>":
	ctypedef struct RSA:
		#/*
		# * The first parameter is used to pickup errors where this is passed
		# * instead of aEVP_PKEY, it is set to 0
		#
		int pad;
		long version;
		const void* meth;
		# /* functional reference if 'meth' is ENGINE-provided */
		void* engine;
		BIGNUM *n;
		BIGNUM *e;
		BIGNUM *d;
		BIGNUM *p;
		BIGNUM *q;
		BIGNUM *dmp1;
		BIGNUM *dmq1;
		BIGNUM *iqmp;

cdef class Crypt:
	cdef RSA* parent_key # Client-wide private key
	
	# Could be empty if we haven't made the exchange yet
	cdef RSA* server_public_key # Server-side encryption key
	cdef int key_size # Buffer variable for quick access
	
	cdef int generate_key(self, size)
	cdef DynamicBuffer decrypt(self, DynamicBuffer b)
	cdef DynamicBuffer encrypt(self, DynamicBuffer b)
	
	cdef handle_connect(self, Socket s)
	
	# Private functions
	#cdef send_public(self, Socket s)
	#cdef recv_public(self, Socket s)