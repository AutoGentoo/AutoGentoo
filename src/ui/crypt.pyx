from libc.stdlib cimport malloc, realloc, free

cdef class Crypt:
	def __init__(self, size):
		self.parent_key = NULL
		self.server_public_key = NULL
		self.key_size = size
		
		if not self.generate_key(size):
			print("Failed to generate RSA key")
	
	cdef int generate_key(self, size):
		cdef int ret = 0;
		cdef BIGNUM* bne = NULL;
		
		cdef int bits = size;
		cdef unsigned long e = RSA_F4;
		
		# // 1. generate rsa key
		cdef bne = BN_new();
		cdef ret = BN_set_word(bne, e);
		if ret != 1:
			RSA_free(self.parent_key)
			BN_free(bne)
			return ret == 1
		
		self.parent_key = RSA_new()
		ret = RSA_generate_key_ex(self.parent_key, bits, bne, NULL)
		if ret != 1:
			RSA_free(self.parent_key)
			BN_free(bne)
			return ret == 1
		
		return ret == 1
	
	cdef DynamicBuffer decrypt(self, DynamicBuffer b):
		pass
	
	cdef DynamicBuffer encrypt(self, DynamicBuffer b):
		pass
	
	cdef handle_connect(self, Socket s):
		cdef rsa_t status = -1;
		s.recv_into(&status, sizeof(rsa_t))
		
		if status == AUTOGENTOO_RSA_VERIFY:
		
		
		if status == AUTOGENTOO_RSA_NOAUTH:
			self.send_public(s)
		
		if self.server_public_key == NULL:
			status = AUTOGENTOO_RSA_NOAUTH
			s.c_send(&status, sizeof(rsa_t), False)
			
			
			
			
		else:
			status = AUTOGENTOO_RSA_AUTH_CONTINUE
	
	# Private functions
	cdef send_public(self, Socket s):
		pass
	
	cdef recv_public(self, Socket s):
		cdef BIO* public_bio_recieve = BIO_new(BIO_s_mem());
		cdef size_t recv_size;
		cdef char* recv_buffer;
		
		# Get size of public key
		s.recv_into(&recv_size, sizeof(size_t))
		
		# Read the public key from socket
		recv_buffer = malloc (recv_size + 1)
		s.recv_into(recv_buffer, recv_size)
		
		# Convert raw data into a BIO
		BIO_read(public_bio_recieve, recv_buffer, <int>recv_size)
		
		# BIO to RSA struct
		self.server_public_key = RSA_new()
		self.server_public_key = PEM_read_bio_RSA_PUBKEY(public_bio_recieve, &self.server_public_key, NULL, NULL)
		
		free (recv_buffer)
	
	#cdef int rsa_verify(self, Socket s)