#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <autogentoo/hacksaw/tools.h>

#include <autogentoo/api/ssl_wrap.h>
#include <autogentoo/api/request_generate.h>

int prv_ssocket_connect(char* hostname, unsigned short port) {
	char str_port[8];
	sprintf(str_port, "%d", port);
	
	
	struct addrinfo* resolved;
	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // TCP ONLY
	
	
	if ((getaddrinfo(hostname, str_port, &hints, &resolved)) != 0) {
		lerror("Cannot resolve hostname: %s", hostname);
		return -1;
	}
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sockfd, resolved->ai_addr, sizeof(struct sockaddr)) == -1 ) {
		lerror("Cannot connect to host %s [%s] on port %d.\n", hostname, resolved->ai_canonname, port);
		return -1;
	}
	freeaddrinfo(resolved);
	
	return sockfd;
}

#define SSOCKET_ERROR() { \
    int err = SSL_get_error(out_sock->ssl, ret); \
    if(err == SSL_ERROR_SSL){ \
        char msg[1024]; \
        ERR_error_string_n(ERR_get_error(), msg, sizeof(msg)); \
        printf("%s %s %s %s\n", msg, ERR_lib_error_string(0), ERR_func_error_string(0), ERR_reason_error_string(0)); \
    } \
}

SSocket* ssocket_new(char* server_hostname, unsigned short port) {
	/* Initialize ssl */
	//autogentoo_client_ssl_init();
	
	SSocket* out_sock = malloc(sizeof(SSocket));
	out_sock->hostname = strdup(server_hostname);
	out_sock->port = port;
	out_sock->cert = NULL;
	
	out_sock->context = SSL_CTX_new(SSLv23_method());
	if (!out_sock->context) {
		lerror("Error creating client context\n");
		ERR_print_errors_fp(stderr);
		goto error;
	}
	
	out_sock->ssl = SSL_new(out_sock->context);
	out_sock->socket = prv_ssocket_connect(out_sock->hostname, out_sock->port);
	if (out_sock->socket == -1) {
		lerror("Failed to connect to server");
		goto error;
	}
	
	SSL_set_fd(out_sock->ssl, out_sock->socket);
	
	int ret;
	if ((ret = SSL_connect(out_sock->ssl)) != 1) {
		lerror("Failed to run SSL_connect()");
		SSOCKET_ERROR();
		goto error;
	}
	
	out_sock->cert = SSL_get_peer_certificate(out_sock->ssl);
	if (out_sock->cert == NULL) {
		lerror("Failed to retrieve certificate from server");
		goto error;
	}
	
	out_sock->cert_name = X509_get_subject_name(out_sock->cert);
	
	return out_sock;
	
error:
	ssocket_free(out_sock);
	return NULL;
}

void ssocket_free(SSocket* ptr) {
	SSL_free(ptr->ssl);
	close(ptr->socket);
	if (ptr->cert)
		X509_free(ptr->cert);
	SSL_CTX_free(ptr->context);
	free(ptr->hostname);
	free(ptr);
}

void autogentoo_client_ssl_init() {
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	if (SSL_library_init() < 0) {
		lerror("Could not initialize ssl library");
		exit(1);
	}
}

void ssocket_request(SSocket* sock, ClientRequest* request) {
	if (!request->ptr)
		client_request_generate(request);
	
	int out_size = 0;
	if ((out_size = SSL_write(sock->ssl, request->ptr, (int)request->size)) != request->size) {
		lerror("Error writing to server");
		lerror("Expected to write %d, wrote %d", request->size, out_size);
	}
}

ssize_t ssocket_read_response(SSocket* sock, void** dest) {
	struct timeval tv = {0, 500};
	setsockopt(sock->socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
	
	/* Read the request */
	size_t chunk_len = 128;
	size_t buffer_size = chunk_len * 2;
	ssize_t total_read = 0, current_bytes = 0;
	
	(*dest) = malloc(buffer_size);
	do {
		if (total_read + chunk_len >= buffer_size) {
			buffer_size *= 2;
			(*dest) = realloc(*dest, buffer_size);
		}
		
		current_bytes = SSL_read(sock->ssl, (*dest) + total_read, (int)chunk_len);
		total_read += current_bytes;
	} while (current_bytes);
	
	return total_read;
}

ssize_t ssocket_read(SSocket* ptr, void* dest, size_t n) {
	return SSL_read(ptr->ssl, dest, (int)n);
}