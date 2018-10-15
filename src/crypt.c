//
// Created by atuser on 9/20/18.
//

#include <autogentoo/server.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <autogentoo/crypt.h>
#include <string.h>
#include <openssl/err.h>

char* rsa_base64(const unsigned char* input, int length, size_t* base64_len) {
	BIO* bmem, * b64;
	BUF_MEM* bptr;
	
	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);
	
	*base64_len = bptr->length;
	char* buff = (char*) malloc(bptr->length);
	memcpy(buff, bptr->data, bptr->length - 1);
	buff[bptr->length - 1] = 0;
	
	BIO_free_all(b64);
	
	return buff;
}

int rsa_binding_verify(Server* parent, RSA* target, Connection* conn) {
	rsa_t c_rsa_request = AUTOGENTOO_RSA_VERIFY;
	write(conn->fd, &c_rsa_request, sizeof(rsa_t));
	
	char* autogentoo_verify = "AutoGentooVerify";
	
	rsa_t rsa_response = -1;
	conn->public_key = target;
	
	rsa_send(conn, autogentoo_verify, 16);
	rsa_recv(conn, &rsa_response);
	
	if (rsa_response == AUTOGENTOO_RSA_CORRECT)
		return 0;
	
	fprintf(stderr, "key verification failed\n");
	conn->public_key = NULL;
	return 1;
}

int rsa_ip_bind(Server* parent, Connection* conn, char* rsa_raw) {
	small_map_insert(parent->rsa_binding, conn->ip, strdup(rsa_raw));
	return 0;
}

int rsa_perform_handshake(Connection* conn) {
	BIO* public_bio_send = BIO_new(BIO_s_mem());
	BIO* public_bio_recieve = BIO_new(BIO_s_mem());
	
	FILE* conn_fp = fdopen(conn->fd, "rw");
	PEM_write_bio_RSAPublicKey(public_bio_send, conn->parent->private_key);
	
	size_t send_len, rec_len;
	send_len = (size_t)BIO_pending(public_bio_send);
	
	rsa_t c_rsa_request = AUTOGENTOO_RSA_NOAUTH;
	write(conn->fd, &c_rsa_request, sizeof(rsa_t));
	read(conn->fd, &rec_len, sizeof(rsa_t));
	
	char* rec_pub = malloc(rec_len + 1);
	char* send_pub = malloc(send_len + 1);
	
	BIO_write(public_bio_send, send_pub, (int)send_len);
	read(conn->fd, rec_pub, rec_len);
	rec_pub[rec_len] = '\0';
	
	write(conn->fd, &send_len, sizeof(size_t));
	write(conn->fd, send_pub, send_len);
	BIO_read(public_bio_recieve, rec_pub, (int)rec_len);
	
	rsa_ip_bind(conn->parent, conn, rec_pub);
	return rsa_binding_verify(conn->parent, conn->public_key, conn);
}

int rsa_load_binding (Connection*) {

}

ssize_t rsa_send(Connection* conn, void* data, size_t size) {
	unsigned char* encrypt = malloc((size_t) RSA_size(conn->public_key));
	int encrypt_len;
	char* err = malloc(130);
	if ((encrypt_len =
			     RSA_public_encrypt((int) size + 1, (unsigned char*) data,
			                        encrypt, conn->public_key, RSA_PKCS1_OAEP_PADDING)) == -1) {
		ERR_load_crypto_strings();
		ERR_error_string(ERR_get_error(), err);
		fprintf(stderr, "Error encrypting message: %s\n", err);
	}
	
	size_t base64_len;
	char* encrypt_base64 = rsa_base64(encrypt, encrypt_len, &base64_len);
	
	write(conn->fd, &encrypt_len, sizeof(int));
	ssize_t sent = write(conn->fd, encrypt_base64, base64_len);
	free(encrypt);
	free(encrypt_base64);
	
	return sent;
}

ssize_t rsa_recv(Connection* conn, void* data_buffer) {
	int encrypt_len;
	read(conn->fd, &encrypt_len, sizeof(int));
	
	char* err = malloc(130);
	char* decrypt = malloc((size_t) RSA_size(conn->parent->private_key));
	if (RSA_private_decrypt(encrypt_len, (unsigned char*) data_buffer, (unsigned char*) decrypt,
	                        conn->parent->private_key, RSA_PKCS1_OAEP_PADDING) == -1) {
		ERR_load_crypto_strings();
		ERR_error_string(ERR_get_error(), err);
		fprintf(stderr, "Error decrypting message: %s\n", err);
		return 0;
	}
	
	free(err);
	return encrypt_len;
}

int rsa_generate(Server* parent) {
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
