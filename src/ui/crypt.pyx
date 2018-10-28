cdef class Crypt:
	def __init__(self, size):
		self.parent_key = NULL
		self.server_public_key = NULL
		self.key_size = size
		
		self.generate_key(size)
	
	cdef int generate_key(self, size):
		cdef int ret = 0;
		cdef RSA* r = NULL;
		cdef BIGNUM* bne = NULL;
		cdef BIO* bp_public = NULL, * bp_private = NULL;
		
		cdef int bits = AUTOGENTOO_RSA_BITS;
		cdef unsigned long e = RSA_F4;
		
		# // 1. generate rsa key
		cdef bne = BN_new();
		cdef ret = BN_set_word(bne, e);
		if ret != 1:
			BIO_free_all(bp_public);
			BIO_free_all(bp_private);
			RSA_free(r);
			BN_free(bne);
			return ret == 1
		
		r = RSA_new();
		ret = RSA_generate_key_ex(r, bits, bne, NULL);
		if ret != 1:
			BIO_free_all(bp_public);
			BIO_free_all(bp_private);
			RSA_free(r);
			BN_free(bne);
			return ret == 1
		
		# // 2. save public key
		bp_public = BIO_new_file("public.pem", "w+");
		ret = PEM_write_bio_RSAPublicKey(bp_public, r);
		if ret != 1:
			BIO_free_all(bp_public);
			BIO_free_all(bp_private);
			RSA_free(r);
			BN_free(bne);
			return ret == 1
		
		# // 3. save private key
		bp_private = BIO_new_file("private.pem", "w+");
		ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
		return (ret == 1);
	
	cdef DynamicBuffer decrypt(self, DynamicBuffer b)
	cdef DynamicBuffer encrypt(self, DynamicBuffer b)
	
	cdef handle_connect(self, Socket s)
	
	# Private functions
	#cdef send_public(self, Socket s)
	#cdef recv_public(self, Socket s)