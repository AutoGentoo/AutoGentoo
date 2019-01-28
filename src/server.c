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
	
	conn->request = malloc(chunk_len);
	ssize_t total_read = 0, current_bytes = 0;
	
	size_t buffer_size = chunk_len;
	current_bytes = connection_read(conn->request + total_read, chunk_len);
	total_read += current_bytes;
	while (current_bytes == chunk_len) {
		if (total_read + chunk_len >= buffer_size) {
			buffer_size *= 2;
			conn->request = realloc(conn->request, buffer_size);
		}
		
		current_bytes = connection_read(conn->request + total_read, chunk_len);
		total_read += current_bytes;
	}
	
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
	response_t res;
	if (request == NULL) {
		http_send_default(conn, BAD_REQUEST);
		res = BAD_REQUEST;
	}
	else {
		res = request_call(request);
		/* Send the response */
		if (res.len > 0 && request->protocol == PROT_AUTOGENTOO)
			rsend(conn, res);
	}
	
	ldinfo("%s -- %s (%d) <==== %d", conn->ip, res.message, (int)res.code, worker_index);
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
	
	for (i = 0; i != server->stages->n; i++)
		host_template_free((*(HostTemplate***) vector_get(server->stages, i))[1]);
	
	small_map_free(server->stages, 0);
	vector_free(server->hosts);
	free(server->port);
	
	free(server);
}

void server_add_queue (Server* parent, Queue* new) {
	queue_add (parent->queue, new);
	kill (parent->queue->proc_id, SIGUSR1);
}

pid_t server_spawn_worker (Server* parent) {
	parent->queue->proc_id = fork ();
	if (parent->queue->proc_id == 0)
		execl (AUTOGENTOO_WORKER, "", NULL);
	return parent->queue->proc_id;
}

char* server_get_path (Server* parent, char* path) {
	char* out;
	asprintf (&out, "%s/%s", parent->location, path);
	
	return out;
}