//
// Created by atuser on 9/20/18.
//

#include <autogentoo/server.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <autogentoo/crypt.h>

int generate_cipher(Server* parent) {
	int ret = 0;
	RSA* r = NULL;
	BIGNUM* bne = NULL;
	BIO* bp_public = NULL, * bp_private = NULL;
	
	int bits = AUTOGENTOO_RSA_BITS;
	unsigned long e = RSA_F4;
	
	// 1. generate rsa key
	bne = BN_new();
	ret = BN_set_word(bne, e);
	if (ret != 1)
		goto free_all;
	
	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret != 1)
		goto free_all;
	
	// 2. save public key
	bp_public = BIO_new_file("public.pem", "w+");
	ret = PEM_write_bio_RSAPublicKey(bp_public, r);
	if (ret != 1) {
		goto free_all;
	}
	
	// 3. save private key
	bp_private = BIO_new_file("private.pem", "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
	
	// 4. free
	free_all:
	
	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);
	
	return (ret == 1);
}

int rsa_perform_handshake(Connection* conn) {
	return 1;
}
