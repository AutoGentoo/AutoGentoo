//
// Created by atuser on 3/24/19.
//

#include <autogentoo/api/ssl_stream.h>
#include <autogentoo/crypt.h>
#include <string.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <netinet/in.h>
#include <errno.h>
#include <openssl/err.h>

SMWServer* smw_server_new (char* port, char* cert_file, char* rsa_file) {
	SMWServer* server = malloc(sizeof(SMWServer));
	server->port = strdup(port);
	server->cert_file = NULL;
	server->rsa_file = NULL;
	
	server->certificate = NULL;
	server->key_pair = NULL;
	server->context = NULL;
	server->django_callback = NULL;
	
	server->pool_head = NULL;
	pthread_mutex_init(&server->pool_mutex, NULL);
	
	server->pid = 0;
	
	if (!rsa_file)
		rsa_file = "smw.rsa";
	if (access(rsa_file, F_OK) != -1) {
		server->rsa_file = strdup(rsa_file);
		FILE* fp = fopen(server->rsa_file, "r");
		
		if (!fp) {
			lerror("Failed to open RSA file: %s", server->rsa_file);
			goto error;
		}
		server->key_pair = PEM_read_RSAPrivateKey(fp, &server->key_pair, NULL, NULL);
		fclose(fp);
		if (!server->key_pair) {
			lerror("Failed to read RSA from file %s", server->rsa_file);
			goto error;
		}
	}
	else {
		server->rsa_file = strdup(rsa_file);
		
		if (!rsa_generate(&server->key_pair)) {
			lerror ("Failed to generate RSA key");
			goto error;
		}
		
		int rsa_fd = open(server->rsa_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
		FILE* fp = fdopen(rsa_fd, "w+");
		if (!PEM_write_RSAPrivateKey(fp, server->key_pair, NULL, NULL, 0, NULL, NULL)) {
			fclose (fp);
			lerror("Failed to write private key to file");
			goto error;
		}
		fclose (fp);
	}
	
	if (!cert_file)
		cert_file = "smw.cert";
	if (access(cert_file, F_OK) != -1) {
		server->cert_file = strdup(cert_file);
		FILE* fp = fopen(server->cert_file, "r");
		server->certificate = PEM_read_X509(fp, &server->certificate, NULL, NULL);
		fclose(fp);
		if (!server->certificate) {
			lerror("Failed to read certificate from file %s", server->cert_file);
			goto error;
		}
	}
	else {
		server->cert_file = strdup(cert_file);
		x509_generate(2, 120, &server->certificate, server->key_pair);
		if (!server->certificate) {
			free(server->cert_file);
			lerror("Failed to generate certificate");
			goto error;
		}
		
		int cert_fd = open(server->cert_file, O_RDWR | O_CREAT, 0 | S_IRUSR | S_IWUSR | S_IRGRP);
		FILE* fp = fdopen(cert_fd, "w+");
		if (!PEM_write_X509(fp, server->certificate)) {
			fclose (fp);
			lerror("Failed to write certificate to file");
			goto error;
		}
		fclose (fp);
	}
	
	return server;
	
error:
	smw_server_free(server);
	return NULL;
}

void smw_server_free(SMWServer* server) {
	pthread_mutex_destroy(&server->pool_mutex);
	free(server->port);
	
	if (server->cert_file)
		free(server->cert_file);
	if (server->rsa_file)
		free(server->rsa_file);
	
	if (server->certificate)
		X509_free(server->certificate);
	
	if (server->key_pair)
		RSA_free(server->key_pair);
	
	if (server->context)
		SSL_CTX_free(server->context);
	
	server->pool_head = NULL;
	
	free(server);
}

void smw_server_loop(SMWServer* server) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	server->pid = pthread_self();
	server->socket = server_init(server->port);
	
	linfo("Server started on port %s", server->port);
	signal (SIGINT, handle_sigint);
	
	addrlen = sizeof(clientaddr);
	
	while (server->keep_alive) {
		int temp_fd = accept(server->socket, (struct sockaddr*) &clientaddr, &addrlen);
		SMWPool* pool = smw_fork(server, temp_fd);
	}
}

pthread_t smw_server_start(SMWServer* server) {
	pthread_create(&server->pid, NULL, (void *(*) (void *))smw_server_loop, server);
	return server->pid;
}

SMWConnection* smw_server_connect(SMWServer* server, int fd) {
	if (fd < 3) {
		lwarning("accept() error");
		return NULL;
	}
	if (fcntl(fd, F_GETFD) == -1 || errno == EBADF) {
		lwarning("Bad fd on accept()");
		return NULL;
	}
	
	if (!server->django_callback) {
		lerror("Django callback not set");
		return NULL;
	}
	
	SMWConnection* out = malloc(sizeof(SMWConnection));
	out->parent = server;
	
	out->write_side = SSL_new(server->context);
	SSL_set_fd(out->write_side, fd);
	out->write_side_sock = SSL_accept(out->write_side);
	
	if (out->write_side_sock <= 0) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	int server_id_len;
	SSL_read(out->write_side, &server_id_len, sizeof(int));
	char* server_id = malloc((size_t)server_id_len + 1);
	SSL_read(out->write_side, server_id, server_id_len); // Read server id from client (django id)
	
	int request_len;
	SSL_read(out->write_side, &request_len, sizeof(int));
	char* request = malloc((size_t)request_len + 1);
	SSL_read(out->write_side, request, request_len); // Read request from client
	
	linfo("Streaming from %s", server_id);
	char* autogentoo_side_ip = server->django_callback(server_id);
	ssocket_new(&out->read_side, autogentoo_side_ip, 9491);
	free(server_id);
	
	
	SSL_write(out->read_side->ssl, request, request_len); //
	
	
	free(request);
	
	return out;
}

SMWPool* smw_fork(SMWServer* server, int accepted_sock) {
	SMWPool* pool = malloc(sizeof(SMWPool));
	pool->child = smw_server_connect(server, accepted_sock);
	pool->child->thread = pool;
	
	pthread_mutex_lock(&server->pool_mutex);
	if (!pool->child) {
		lerror ("Failed to create connection");
		server->pool_head = pool->next;
		free(pool);
		pthread_mutex_unlock(&server->pool_mutex);
		return NULL;
	}
	
	pool->next = server->pool_head;
	pool->prev = NULL;
	if (pool->next)
		pool->next->prev = pool;
	pthread_mutex_unlock(&server->pool_mutex);
	
	pthread_create(&pool->pid, NULL, (void* (*)(void*))smw_stream, pool->child);
	
	return pool;
}

void smw_stream(SMWConnection* conn) {
	char buffer[SMW_STREAM_BUFFER_SIZE];
	ssize_t current_read = 0;
	
	do {
		current_read = SSL_read(conn->read_side->ssl, (char*)buffer, SMW_STREAM_BUFFER_SIZE);
		SSL_write(conn->write_side, (char*)buffer, SMW_STREAM_BUFFER_SIZE);
	} while (current_read >= SMW_STREAM_BUFFER_SIZE);
	
	ssocket_free(conn->read_side);
	close(conn->write_side_sock);
	SSL_free(conn->write_side);
	
	pthread_mutex_lock(&conn->parent->pool_mutex);
	if (conn->thread->prev)
		conn->thread->prev->next = conn->thread->next;
	else
		conn->parent->pool_head = conn->thread->next;
	pthread_mutex_unlock(&conn->parent->pool_mutex);
	free(conn);
}