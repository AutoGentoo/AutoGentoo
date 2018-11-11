from d_malloc cimport DynamicBuffer
from op_socket cimport Socket

cdef extern from "openssl/bn.h":
	ctypedef struct BIGNUM:
		pass
	
	ctypedef unsigned long BN_ULONG;
	
	BIGNUM *BN_new(void);
	int BN_set_word(BIGNUM *a, BN_ULONG w);
	char *BN_bn2hex(BIGNUM *a)
	int BN_hex2bn(BIGNUM ** a, char *s)
	
	void BN_clear_free(BIGNUM *a);
	void BN_free(BIGNUM *a);

cdef extern from "openssl/bio.h":
	ctypedef void BIO;
	
	void* BIO_s_mem(void);
	BIO *BIO_new(void* type);
	int BIO_read(BIO *b, void *data, int len);
	int BIO_write(BIO *b, void *data, int len);
	size_t BIO_pending (BIO* b)
	void BIO_free_all(BIO *a);
	BIO *BIO_new_file(const char *filename, const char *mode);

cdef extern from "openssl/pem.h":
	int PEM_write_bio_RSAPublicKey(BIO* b, RSA* r);
	int PEM_write_bio_RSAPrivateKey(BIO* b, RSA* r, void* enc, unsigned char* kstr, int klen, void* pem_password, void* u);
	RSA* PEM_read_bio_RSA_PUBKEY(BIO* b, RSA* r, void* password, void* u)

cdef extern from "<openssl/rsa.h>":
	ctypedef void RSA;
	ctypedef void BN_GENCB;
	
	cdef unsigned long RSA_F4 = 0x10001L;
	
	void RSA_free(RSA *r);
	RSA *RSA_new(void);
	int RSA_generate_key_ex(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb);

cdef extern from "<autogentoo/writeconfig.h>":
	cdef enum rsa_t:
		AUTOGENTOO_RSA_VERIFY, #!< Check if we have updated public key
		AUTOGENTOO_RSA_CORRECT, #!< RSA exchange complete
		AUTOGENTOO_RSA_INCORRECT, #!< RSA wrong key
		AUTOGENTOO_RSA_AUTH_CONTINUE, #!< Server has public key, continue with handshake
		AUTOGENTOO_RSA_SERVERSIDE_PUBLIC = 1 << 0,
		AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC = 1 << 1,
		AUTOGENTOO_RSA_NOAUTH = AUTOGENTOO_RSA_SERVERSIDE_PUBLIC | AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC

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
	#cdef rsa_t rsa_verify(self, Socket s)