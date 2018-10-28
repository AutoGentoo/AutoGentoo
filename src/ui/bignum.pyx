cdef extern from "openssl/crypto.h":
	void OPENSSL_free(void *)

cdef pylong_from_bn(BIGNUM *bn):
	cdef char* s
	s = BN_bn2hex(bn)
	n = long(s, 16)
	OPENSSL_free(s)
	return n

cdef pylong_as_bn(object n, BIGNUM ** bn):
	cdef bytes s
	try:
		s = b"%x" % n
	except TypeError:
		raise TypeError("expected an integer")
	BN_hex2bn(bn, s)
