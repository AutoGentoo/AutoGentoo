#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <autogentoo/autogentoo.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>

Server* srv = NULL;

Connection* connection_new (Server* server, int conn_fd) {
	Connection* out = malloc(sizeof(Connection));
	out->parent = server;
	out->fd = conn_fd;
	
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	getpeername(out->fd, (struct sockaddr*) &addr, &addr_size);
	out->ip = strdup(inet_ntoa(addr.sin_addr));
	out->communication_type = COM_PLAIN;
	
	return out;
}

Connection* connection_new_tls(EncryptServer* server, int accepted_fd) {
	Connection* out = connection_new(server->parent, accepted_fd);
	out->communication_type = COM_RSA;
	out->encrypted_connection = SSL_new(server->context);
	SSL_set_fd(out->encrypted_connection, accepted_fd);
	out->encrypted_fd = SSL_accept(out->encrypted_connection);
	
	if (out->encrypted_fd <= 0) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	return out;
}

void server_encrypt_free (EncryptServer* server) {
	free(server->rsa_path);
	free(server->cert_path);
	free(server->port);
	
	X509_free(server->certificate);
	RSA_free(server->key_pair);
	SSL_CTX_free(server->context);
	free(server);
}

void connection_free (Connection* conn) {
	if (conn->status != CLOSED) {
		if (fcntl(conn->fd, F_GETFD) != -1 || errno != EBADF)
			close(conn->fd);
		conn->status = CLOSED;
	}
	
	if (conn->communication_type == COM_RSA)
		SSL_free(conn->encrypted_connection);
	
	free(conn->request);
	free(conn->ip);
	free(conn);
}

void server_recv (Connection* conn) {
	struct timeval tv = {0, 500};
	setsockopt(conn->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
	
	/* Read the request */
	size_t chunk_len = 128;
	size_t buffer_size = chunk_len * 2;
	ssize_t total_read = 0, current_bytes = 0;
	
	conn->request = malloc(buffer_size);
	do {
		if (total_read + chunk_len >= buffer_size) {
			buffer_size *= 2;
			conn->request = realloc(conn->request, buffer_size);
		}
		
		current_bytes = connection_read(conn->request + total_read, chunk_len);
		total_read += current_bytes;
	} while (current_bytes == chunk_len);
	
	if (total_read < 0) { // receive error
		conn->status = SERVER_ERROR;
		return;
	}
	else if (total_read == 0) { // receive socket closed
		conn->status = FAILED;
		return;
	}
	
	conn->size = (size_t) total_read;
	conn->status = CONNECTED;
}

void server_respond(Connection* conn, int worker_index) {
	conn->worker = worker_index;
	
	/* Read from the client and parse request */
	server_recv(conn);
	if (conn->status != CONNECTED) {
		connection_free(conn);
		return;
	}
	Request* request = request_handle(conn);
	
	/* Run the request */
	Response res;
	res.code = OK;
	res.content = dynamic_binary_new(DB_ENDIAN_TARGET_NETWORK);
	if (request == NULL) {
		http_send_default(conn, BAD_REQUEST);
		res.code = BAD_REQUEST;
	}
	else {
		request_call(&res, request);
		/* Send the response */
		/* Response code */
		int code_big_endian = htonl(res.code.code);
		
		size_t write_size = 0;
		write_size += conn_write(conn, &code_big_endian, sizeof(int));
		write_size += conn_write(conn, res.code.message, res.code.len + 1);
		
		/* Response content */
		write_size += conn_write(conn, res.content->template, res.content->template_used_size + 1);
		write_size += conn_write(conn, res.content->ptr, res.content->used_size);
	}
	
	if (request)
		ldinfo("%s -- %s (%d) code = %d", conn->ip, res.code.message, (int)res.code.code, request->request_type);
	else
		ldinfo("%s -- %s (%d)", conn->ip, res.code.message, (int)res.code.code);
	Server* parent = conn->parent;
	
	if (request && request->directive == DIR_CONNECTION_OPEN) {
		request_free(request);
		server_respond(conn, worker_index);
	}
	else
		connection_free(conn);
	
	/* Update the config */
	write_server(parent);
}

Host* server_get_host(Server* server, char* id) {
	int i;
	for (i = 0; i != server->hosts->n; i++) {
		Host* current_host = *(Host**) vector_get(server->hosts, i);
		if (strcmp(id, current_host->id) == 0)
			return current_host;
	}
	
	return NULL;
}

void daemonize (char* _cwd) {
	pid_t pid, sid;
	
	/* already a daemon */
	if (getppid() == 1)
		return;
	
	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
		exit(1);
	
	if (pid > 0) {
		linfo("Forked to pid: %d", (int) pid);
		linfo("Moving to background");
		fflush(stdout);
		exit(0); /*Killing the Parent Process*/
	}
	
	/* At this point we are executing as the child process */
	
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0)
		exit(1);
	
	/* Change the current working directory. */
	if ((chdir(_cwd)) < 0)
		exit(1);
	
	/*resettign File Creation Mask */
	umask(027);
}

void server_free (Server* server) {
	free(server->location);
	
	int i;
	for (i = 0; i != server->hosts->n; i++)
		host_free(*(Host**) vector_get(server->hosts, i));
	
	vector_free(server->hosts);
	map_free(server->auth_tokens, (void (*)(void*))token_free);
	
	free(server->port);
	free(server);
}

char* server_get_path (Server* parent, char* path) {
	char* out;
	asprintf (&out, "%s/%s", parent->location, path);
	
	return out;
}