//
// Created by atuser on 9/23/18.
//

#ifndef AUTOGENTOO_CRYPT_H
#define AUTOGENTOO_CRYPT_H

#include <autogentoo/server.h>

#ifndef AUTOGENTOO_RSA_BITS
#define AUTOGENTOO_RSA_BITS 2048
#endif

int generate_cipher(Server* parent);

int rsa_perform_handshake(Connection* conn);
size_t rsa_send(Connection* conn, void* data, size_t size);
size_t rsa_recv(Connection* conn, void* data_buffer, size_t size);

#endif //AUTOGENTOO_CRYPT_H
