cdef extern from "openssl/bn.h":
	ctypedef struct BIGNUM:
		pass
	
	char *BN_bn2hex(BIGNUM *a)
	int BN_hex2bn(BIGNUM ** a, char *s)
	
	void BN_clear_free(BIGNUM *a)

cdef:
	pylong_from_bn(BIGNUM *bn)
	pylong_as_bn(object n, BIGNUM ** bn)
