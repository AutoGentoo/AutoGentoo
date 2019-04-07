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

int socket_connect(char* hostname, unsigned short port) {
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
		lerror("Cannot connect to host %s on port %d.", hostname, port);
		sockfd = -1;
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

int ssocket_new(SSocket** socket_ptr, char* server_hostname, unsigned short port) {
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
	out_sock->socket = socket_connect(out_sock->hostname, out_sock->port);
	if (out_sock->socket == -1) {
		lerror("Failed to connect to server");
		goto error;
	}
	
	SSL_set_fd(out_sock->ssl, out_sock->socket);
	
	int ret;
	if ((ret = SSL_connect(out_sock->ssl)) != 1) {
		lerror("Failed to run SSL_connect()");
		SSOCKET_ERROR()
		goto error;
	}
	
	out_sock->cert = SSL_get_peer_certificate(out_sock->ssl);
	if (out_sock->cert == NULL) {
		lerror("Failed to retrieve certificate from server");
		goto error;
	}
	
	out_sock->cert_name = X509_get_subject_name(out_sock->cert);
	
	(*socket_ptr) = out_sock;
	return 0;
	
error:
	ssocket_free(out_sock);
	return 1;
}

void ssocket_free(SSocket* ptr) {
	if (ptr->ssl)
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

typedef ssize_t (*prv_socket_func) (void* sock, void* ptr, int size);

void prv_socket_template_send(void* socket, ClientRequest* request, prv_socket_func function) {
	if (!request->ptr)
		client_request_generate(request);
	
	int out_size = function(socket, request->ptr, (int)request->size);
	if (out_size != request->size)
		lerror("Error writing to server\nExpected to write %d, wrote %d", request->size, out_size);
}

void ssocket_request(SSocket* sock, ClientRequest* request) {
	prv_socket_template_send(sock->ssl, request, (ssize_t (*)(void*,void*,int))SSL_write);
}

void socket_request(int sock, ClientRequest* request) {
	prv_socket_template_send((void*)(long)sock, request, (ssize_t (*)(void*,void*,int))write);
}

int prv_socket_template_read(void* sock, void** dest, ssize_t (*function)(void*,void*,int), int is_server) {
	/* Read the request */
	size_t chunk_len = 32;
	size_t buffer_size = chunk_len * 2;
	int total_read = 0, current_bytes = 0;
	
	int sentinal = is_server ? chunk_len : 1;
	
	(*dest) = malloc(buffer_size);
	do {
		if (total_read + chunk_len >= buffer_size) {
			buffer_size *= 2;
			(*dest) = realloc(*dest, buffer_size);
		}
		
		current_bytes = function(sock, (*dest) + total_read, (int)chunk_len);
		total_read += current_bytes;
	} while (current_bytes >= sentinal);
	return total_read;
}

int ssocket_read(SSocket* sock, void** dest, int is_server) {
	return prv_socket_template_read(sock->ssl, dest, (ssize_t (*)(void*,void*,int))SSL_read, is_server);
}

int socket_read(int sock, void** dest, int is_server) {
	/* Read the request */
	size_t chunk_len = 32;
	size_t buffer_size = chunk_len * 2;
	int total_read = 0, current_bytes = 0;
	
	(*dest) = malloc(buffer_size);
	do {
		if (total_read + chunk_len >= buffer_size) {
			buffer_size *= 2;
			(*dest) = realloc(*dest, buffer_size);
		}
		
		current_bytes = read(sock, (*dest) + total_read, (int)chunk_len);
		total_read += current_bytes;
	} while (current_bytes == chunk_len);
	return total_read;
}