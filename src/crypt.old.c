//
// Created by atuser on 11/18/18.
//


int rsa_recv_public(Connection* conn) {
	BIO* public_bio_recieve = BIO_new(BIO_s_mem());
	
	/* Get the length of the pub_key */
	size_t rec_len;
	read(conn->fd, &rec_len, sizeof(rsa_t));
	char* rec_pub = malloc(rec_len + 1);
	
	/* Read the public key into the buffer */
	read(conn->fd, rec_pub, rec_len);
	rec_pub[rec_len] = '\0';
	
	/* Convert raw data into a BIO */
	BIO_read(public_bio_recieve, rec_pub, (int)rec_len);
	
	conn->public_key = RSA_new();
	conn->public_key = PEM_read_bio_RSA_PUBKEY(public_bio_recieve, &conn->public_key, NULL, NULL);
	
	free(rec_pub);
	BIO_free_all(public_bio_recieve);
	
	return rsa_ip_bind(conn->parent, conn, rec_pub, (int)rec_len);
}
int rsa_send_public(Connection* conn) {
	BIO* public_bio_send = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPublicKey(public_bio_send, conn->parent->rsa_child->private_key);
	size_t send_len = (size_t)BIO_pending(public_bio_send);
	
	char* send_pub = malloc(send_len + 1);
	BIO_write(public_bio_send, send_pub, (int)send_len);
	
	write(conn->fd, &send_len, sizeof(size_t));
	write(conn->fd, send_pub, send_len);
	free(send_pub);
	BIO_free_all(public_bio_send);
	
	return 0;
}

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

rsa_t rsa_binding_verify(Server* parent, RSA* target, Connection* conn) {
	rsa_t c_rsa_request = AUTOGENTOO_RSA_VERIFY;
	write(conn->fd, &c_rsa_request, sizeof(rsa_t));
	
	char* autogentoo_verify = "AutoGentooVerify";
	rsa_t rsa_response = -1;
	rsa_t exit_code = 0;
	
	int rsa_size = RSA_size(conn->public_key);
	unsigned char* encrypt = malloc((size_t)rsa_size);
	char* err = malloc(130);
	
	int encrypt_len;
	if ((encrypt_len =
			     RSA_public_encrypt(rsa_size, (unsigned char*)autogentoo_verify,
			                        encrypt, target, RSA_PKCS1_OAEP_PADDING)) == -1) {
		ERR_load_crypto_strings();
		ERR_error_string(ERR_get_error(), err);
		fprintf(stderr, "Error encrypting message: %s\n", err);
		
		free(err);
		free(encrypt);
		return -1;
	}
	
	size_t base64_len;
	char* encrypt_base64 = rsa_base64(encrypt, encrypt_len, &base64_len);
	
	write_int_fd(conn->fd, encrypt_len);
	write(conn->fd, encrypt_base64, base64_len);
	
	free(encrypt_base64);
	free(err);
	free(encrypt);
	
	read(conn->fd, &rsa_response, sizeof(int));
	if (rsa_response == AUTOGENTOO_RSA_INCORRECT) {
		fprintf(stderr, "Server has wrong client public key");
		exit_code |= AUTOGENTOO_RSA_SERVERSIDE_PUBLIC;
	}
	
	size_t incoming_size;
	read(conn->fd, &incoming_size, sizeof(size_t));
	
	char* raw_encrypted = malloc (incoming_size);
	char* decrypted = malloc(17);
	read(conn->fd, raw_encrypted, incoming_size);
	
	rsa_decrypt(conn, raw_encrypted, decrypted, (int)incoming_size, 16);
	if (strncmp(decrypted, autogentoo_verify, 16) != 0) {
		fprintf(stderr, "Client has wrong server public key");
		
		rsa_response = AUTOGENTOO_RSA_INCORRECT;
		exit_code |= AUTOGENTOO_RSA_SERVERSIDE_PUBLIC;
	}
	else
		rsa_response = AUTOGENTOO_RSA_CORRECT;
	write(conn->fd, &rsa_response, sizeof(int));
	free(raw_encrypted);
	free(decrypted);
	
	return exit_code;
}

int rsa_ip_bind(Server* parent, Connection* conn, char* rsa_raw, int len) {
	struct {
		char* rsa_raw;
		int rsa_len;
	} rsa_binding = {strdup(rsa_raw), len};
	
	small_map_insert(parent->rsa_child->rsa_binding, conn->ip, &rsa_binding);
	return 0;
}

int rsa_perform_handshake(Connection* conn) {
	int ret = 0;
	rsa_t c_rsa_request = rsa_load_binding(conn);
	
	/* Check if client and server have the same status */
	int check_status;
	write(conn->fd, &c_rsa_request, sizeof(rsa_t));
	read(conn->fd, &check_status, sizeof(int));
	if (!check_status) {
		fprintf(stderr, "Client and server don't have same status code\n");
		return 0;
	}
	
	if (c_rsa_request & AUTOGENTOO_RSA_SERVERSIDE_PUBLIC)
		ret += rsa_recv_public(conn);
	
	if (c_rsa_request & AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC)
		ret += rsa_send_public(conn);
	
	return ret == 0;
}

rsa_t rsa_load_binding(Connection* conn) {
	struct {
		char* rsa_raw;
		int rsa_len;
	} *public_raw = small_map_get(conn->parent->rsa_child->rsa_binding, conn->ip);
	if (!public_raw) {
		rsa_t send_binding_status = AUTOGENTOO_RSA_NOAUTH;
		write(conn->fd, &send_binding_status, sizeof(rsa_t));
		return AUTOGENTOO_RSA_NOAUTH;
	}
	
	BIO* target_public_bio = BIO_new(BIO_s_mem());
	BIO_read(target_public_bio, public_raw, public_raw->rsa_len);
	conn->public_key = PEM_read_bio_RSA_PUBKEY(target_public_bio, &conn->public_key, NULL, NULL);
	BIO_free_all(target_public_bio);
	
	return rsa_binding_verify(conn->parent, conn->public_key, conn);
}

ssize_t rsa_send(Connection* conn, void* data, size_t size) {
	/**
	 * [int: chunknum]
	 * { chunk
	 *   [int: encrypted size (base64)]
	 *   [int: decrypted size (x <= key size)]
	 *   [encrypted size bytes of encrypted data (base64)]
	 * }
	 */
	
	int rsa_size = RSA_size(conn->public_key);
	int chunk_number = (int)size / rsa_size;
	unsigned char* encrypt = malloc((size_t)rsa_size);
	char* err = malloc(130);
	size_t sent = 0;
	size_t offset = 0;
	
	sent += write_int_fd(conn->fd, chunk_number);
	for (; chunk_number >= 0; chunk_number--) {
		int chunk_size = chunk_number == 0 ? (int)size % rsa_size : rsa_size;
		int encrypt_len;
		if ((encrypt_len =
				     RSA_public_encrypt(chunk_size, (unsigned char*) data + offset,
				                        encrypt, conn->public_key, RSA_PKCS1_OAEP_PADDING)) == -1) {
			ERR_load_crypto_strings();
			ERR_error_string(ERR_get_error(), err);
			fprintf(stderr, "Error encrypting message: %s\n", err);
			
			free(err);
			free(encrypt);
			return 0;
		}
		
		size_t base64_len;
		char* encrypt_base64 = rsa_base64(encrypt, encrypt_len, &base64_len);
		
		sent += write_int_fd(conn->fd, encrypt_len);
		sent += write_int_fd(conn->fd, chunk_size);
		sent += write(conn->fd, encrypt_base64, base64_len);
		
		offset += chunk_size;
		free(encrypt_base64);
	}
	
	free(err);
	free(encrypt);
	return sent;
}

ssize_t rsa_decrypt(Connection* conn, void* from, void* to, int encrypted_size, int decrypted_size) {
	char* err = malloc(130);
	decrypted_size = RSA_private_decrypt(
			encrypted_size,
			(unsigned char*) from,
			(unsigned char*) to,
			conn->parent->rsa_child->private_key,
			RSA_PKCS1_OAEP_PADDING);
	if (decrypted_size == -1) {
		ERR_load_crypto_strings();
		ERR_error_string(ERR_get_error(), err);
		fprintf(stderr, "Error decrypting message: %s\n", err);
	}
	
	free(err);
	return decrypted_size;
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
	if (ret != 1) {
		fprintf(stderr, "bignum generation failed\n");
		goto free_all;
	}
	
	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret != 1) {
		fprintf(stderr, "RSA key generation failed\n");
		goto free_all;
	}
	
	// 2. save public key
	bp_public = BIO_new_file("public.pem", "w+");
	ret = PEM_write_bio_RSAPublicKey(bp_public, r);
	if (ret != 1)
		goto free_all;
	
	// 3. save private key
	bp_private = BIO_new_file("private.pem", "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL) == 1;
	
	// 4. free
	free_all:
	
	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);
	
	return (ret != 1);
}