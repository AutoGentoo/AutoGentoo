//
// Created by atuser on 2/18/19.
//

#ifndef AUTOGENTOO_SSL_WRAP_H
#define AUTOGENTOO_SSL_WRAP_H

#include <autogentoo/request.h>
#include "request_generate.h"

typedef struct __SSocket SSocket;

struct __SSocket {
	SSL* ssl;
	X509* cert;
	X509_NAME* cert_name;
	SSL_CTX* context;
	
	char* hostname;
	unsigned short port;
	int socket;
};

SSocket* ssocket_new(char* server_hostname, unsigned short port);
void ssocket_free(SSocket* ptr);
void autogentoo_client_ssl_init();
void ssocket_request(SSocket* ptr, ClientRequest* request);
ssize_t ssocket_read_response(SSocket* sock, void** dest);
ssize_t ssocket_read(SSocket* ptr, void* dest, size_t n);

#endif //AUTOGENTOO_SSL_WRAP_H
