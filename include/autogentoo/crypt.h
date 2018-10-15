//
// Created by atuser on 9/23/18.
//

#ifndef AUTOGENTOO_CRYPT_H
#define AUTOGENTOO_CRYPT_H

#include <autogentoo/server.h>

#ifndef AUTOGENTOO_RSA_BITS
#define AUTOGENTOO_RSA_BITS 2048
#endif

typedef enum {
	AUTOGENTOO_RSA_NOAUTH, //!< Needs public key from client
	AUTOGENTOO_RSA_VERIFY, //!< Check if we have updated public key
	AUTOGENTOO_RSA_CORRECT, //!< RSA exchange complete
	AUTOGENTOO_RSA_INCORRECT //!< RSA wrong key
} rsa_t;

int rsa_binding_verify(Server* parent, RSA* target, Connection* conn);
int rsa_ip_bind(Server* parent, Connection* conn, char* rsa_raw, int len);
int rsa_generate(Server* parent);
int rsa_perform_handshake(Connection* conn);
ssize_t rsa_send(Connection* conn, void* data, size_t size);
ssize_t rsa_recv(Connection* conn, void* data_buffer);
char* rsa_base64(const unsigned char* input, int length, size_t* base64_len);
int rsa_load_binding (Connection* conn);

#endif //AUTOGENTOO_CRYPT_H
