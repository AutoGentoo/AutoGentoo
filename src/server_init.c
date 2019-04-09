//
// Created by atuser on 11/20/18.
//

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <autogentoo/autogentoo.h>
#include <autogentoo/crypt.h>
#include <string.h>
#include "worker/worker.h"

int server_init (char* port) {
	struct addrinfo hints, * res, * p;
	int listenfd = -1;
	
	// getaddrinfo for host
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &res) != 0) {
		lerror("getaddrinfo() error");
		exit(1);
	}
	// socket and bind
	for (p = res; p != NULL; p = p->ai_next) {
		listenfd = socket(p->ai_family, p->ai_socktype, 0);
		if (listenfd == -1)
			continue;
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
			lerror("setsockopt(SO_REUSEADDR) failed");
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
	}
	if (p == NULL) {
		lerror("socket() or bind()");
		switch (errno) {
			case EADDRINUSE:
				lerror("Port %s in use", port);
				break;
			case EACCES:
				lerror("Permission denied");
				break;
			case ENETUNREACH:
				lerror("Network unreachable");
				break;
			default:
				break;
		}
		exit(1);
	}
	
	freeaddrinfo(res);
	
	// listen for incoming connections
	
	if (listen(listenfd, 64) != 0) {
		lerror("listen() error");
		exit(1);
	}
	
	return listenfd;
}

Server* server_new (char* location, char* port, server_t opts) {
	Server* out = malloc(sizeof(Server));
	
	out->hosts = vector_new(sizeof(Host*), VECTOR_REMOVE | VECTOR_UNORDERED);
	out->location = strdup(location);
	out->autogentoo_org_token = NULL;
	
	out->worker_handler = worker_handler_new();
	
	chdir(out->location);
	out->opts = opts;
	out->port = strdup(port);
	out->auth_tokens = map_new(128, 0.8);
	
	return out;
}

EncryptServer* server_encrypt_new(Server* parent, char* port, char* cert_path, char* rsa_path, enc_server_t opts) {
	EncryptServer* out = malloc (sizeof (EncryptServer));
	
	out->parent = parent;
	out->port = strdup (port);
	out->opts = opts;
	
	out->certificate = NULL;
	out->key_pair = NULL;
	out->cert_path = cert_path;
	out->rsa_path = rsa_path;
	
	if (!out->cert_path)
		out->cert_path = server_get_path(parent, "certificate.pem");
	if (!out->rsa_path)
		out->rsa_path = server_get_path(parent, "private.pem");
	
	if (x509_generate_write(out) != 0) {
		lerror ("Failed to initialize certificates");
		free(out->port);
		free(out->cert_path);
		free(out->rsa_path);
		free (out);
		return NULL;
	}
	
	OpenSSL_add_ssl_algorithms();
	SSL_load_error_strings();
	OpenSSL_add_all_ciphers();
	
	out->context = SSL_CTX_new(SSLv23_server_method());
	if (!out->context) {
		lerror("Error creating server context\n");
		ERR_print_errors_fp(stderr);
		exit(1);
	}
	
	if (!SSL_CTX_use_certificate(out->context, out->certificate)) {
		lerror("Failed to load certificate into SSL context");
		ERR_print_errors_fp(stderr);
		exit(1);
	}
	if (!SSL_CTX_use_RSAPrivateKey(out->context, out->key_pair)) {
		lerror("Failed to load private into SSL context");
		ERR_print_errors_fp(stderr);
		exit(1);
	}
	
	return out;
}