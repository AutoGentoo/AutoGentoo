from libc.stdlib cimport malloc, free
from libc.string cimport strcmp, strlen

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
			status = self.rsa_verify(s)
		
		cdef rsa_t check_status;
		cdef int send_status = 1;
		s.recv_into(&check_status, sizeof(rsa_t))
		if check_status != status:
			send_status = 0
		s.c_send(&send_status, sizeof (int), False)
		
		if send_status == 0:
			print("Client and server don't have same status code")
			return
		
		if status & AUTOGENTOO_RSA_SERVERSIDE_PUBLIC:
			self.send_public(s)
		
		if status & AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC:
			self.recv_public(s)
	
	# Private functions
	cdef send_public(self, Socket s):
		cdef BIO* public_bio_send = BIO_new(BIO_s_mem());
		PEM_write_bio_RSAPublicKey(public_bio_send, self.parent_key)
		cdef size_t send_len = <size_t>BIO_pending(public_bio_send)
		
		cdef char* send_pub = malloc(send_len + 1);
		BIO_write(public_bio_send, send_pub, <int>send_len)
		
		s.c_send(&send_len, sizeof(size_t))
		s.c_send(send_pub, send_len)
		
		free(send_pub)
		BIO_free_all(public_bio_send)
	
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
		BIO_free_all(public_bio_recieve)
	
	cdef rsa_t rsa_verify(self, Socket s):
		cdef rsa_t rsa_response = -1;
		cdef rsa_t response = 0
		
		# Get size of the encrypted data
		cdef size_t incoming_size;
		s.recv_into(&incoming_size, sizeof(size_t))
		
		cdef void* encrypted = malloc (incoming_size);
		s.recv_into(encrypted, incoming_size)
		
		cdef DynamicBuffer decrypted_temp = self.decrypt(DynamicBuffer.new_from_initial(encrypted, 
incoming_size))
		free(encrypted)
		if strcmp(<char*>decrypted_temp.ptr, "AutoGentooVerify") == 0:
			rsa_response = AUTOGENTOO_RSA_CORRECT
		else:
			rsa_response = AUTOGENTOO_RSA_INCORRECT
			response |= AUTOGENTOO_RSA_SERVERSIDE_PUBLIC
		s.c_send(&rsa_response, sizeof(int), False)
		
		cdef DynamicBuffer decrypted_send = DynamicBuffer("AutogentooVerify");
		if self.server_public_key == NULL:
			response |= AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC
			
			cdef char* arbitrary_send = "No server public"
			cdef size_t arbitrary_len = strlen(arbitrary_send)
			
			s.c_send(&arbitrary_len, sizeof(size_t))
			s.c_send(arbitrary_send, arbitrary_len)
		else:
			s.c_send(&decrypted_send.n, sizeof(int))
			s.send(self.encrypt(decrypted_send), False)
			
			s.recv_into(&rsa_response, sizeof(int))
			if rsa_response == AUTOGENTOO_RSA_INCORRECT:
				response |= AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC
		
		return response
