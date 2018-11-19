//
// Created by atuser on 9/23/18.
//

#ifndef AUTOGENTOO_CRYPT_H
#define AUTOGENTOO_CRYPT_H

#include <autogentoo/server.h>

#ifndef AUTOGENTOO_RSA_BITS
#define AUTOGENTOO_RSA_BITS 2048
#endif

/* OLD */
/*
typedef enum {
	AUTOGENTOO_RSA_VERIFY, //!< Check if we have updated public key
	AUTOGENTOO_RSA_CORRECT, //!< RSA exchange complete
	AUTOGENTOO_RSA_INCORRECT, //!< RSA wrong key
	AUTOGENTOO_RSA_AUTH_CONTINUE, //!< Server has public key, continue with handshake
	
	AUTOGENTOO_RSA_SERVERSIDE_PUBLIC = 1 << 0,
	AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC = 1 << 1,
	AUTOGENTOO_RSA_NOAUTH = AUTOGENTOO_RSA_SERVERSIDE_PUBLIC | AUTOGENTOO_RSA_CLIENTSIDE_PUBLIC
} rsa_t;

int rsa_recv_public(Connection* conn);
int rsa_send_public(Connection* conn);

rsa_t rsa_binding_verify(Server* parent, RSA* target, Connection* conn);
int rsa_ip_bind(Server* parent, Connection* conn, char* rsa_raw, int len);
int rsa_generate(Server* parent);
int rsa_perform_handshake(Connection* conn);
ssize_t rsa_send(Connection* conn, void* data, size_t size);
ssize_t rsa_decrypt(Connection* conn, void* from, void* to, int encrypted_size, int decrypted_size);
char* rsa_base64(const unsigned char* input, int length, size_t* base64_len);
rsa_t rsa_load_binding(Connection* conn);
*/

/* OpenSSL Certificate */
void x509_generate(int serial, int days_valid, X509** cert_out, RSA* key_pair);
int x509_generate_write(EncryptServer* parent);

#endif //AUTOGENTOO_CRYPT_H
