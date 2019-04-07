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

int ssocket_new(SSocket** socket_ptr, char* server_hostname, unsigned short port);
void ssocket_free(SSocket* ptr);
void autogentoo_client_ssl_init();

int socket_connect(char* hostname, unsigned short port);

void ssocket_request(SSocket* ptr, ClientRequest* request);
void socket_request(int sock, ClientRequest* request);

int ssocket_read(SSocket* sock, void** dest, int is_server);
int socket_read(int sock, void** dest, int is_server);

#endif //AUTOGENTOO_SSL_WRAP_H
