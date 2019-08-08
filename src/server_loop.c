//
// Created by atuser on 11/20/18.
//

#define _GNU_SOURCE

#include <autogentoo/server.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <autogentoo/writeconfig.h>
#include <sys/socket.h>
#include "autogentoo/worker.h"

int prv_random_string(char* out, size_t len);

Connection* accept_conn (void* server, int fd, com_t type) {
	if (fd < 3) {
		lwarning("accept() error");
		return NULL;
	}
	if (fcntl(fd, F_GETFD) == -1 || errno == EBADF) {
		lwarning("Bad fd on accept()");
		return NULL;
	}
	
	Connection* out = NULL;
	if (type == COM_RSA)
		out = connection_new_tls((EncryptServer*)server, fd);
	if (type == COM_PLAIN)
		out = connection_new((Server*)server, fd);
	signal(SIGPIPE, SIG_IGN);
	
	return out;
}

void server_start (Server* server) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	server->pthread = pthread_self();
	server->socket = server_init(9490);
	
	linfo("Server started on port %s", server->port);
	
	if (server->opts & DAEMON)
		daemonize(server->location);
	
	server->pid = getpid();
	srv = server;
	signal (SIGINT, handle_sigint);
	
	addrlen = sizeof(clientaddr);
	server->keep_alive = 1;
#ifndef AUTOGENTOO_NO_THREADS
	server->pool_handler = pool_handler_new(32);
#endif
	
	server->job_handler = worker_handler_new();
	
	AccessToken org_creation_token;
	org_creation_token.host_id = NULL;
	org_creation_token.access_level = TOKEN_SERVER_AUTOGENTOO_ORG;
	org_creation_token.user_id = "autogentoo.org";
	
	if (!server->autogentoo_org_token)
		server->autogentoo_org_token = strdup(auth_issue_token(server, &org_creation_token)->auth_token);
	if (!server->autogentoo_org_token) {
		lerror ("Failed to generate auth_token");
		lerror ("You can not register this server without this token");
	}
	else {
		linfo ("Your token is:");
		linfo ("%s", server->autogentoo_org_token);
		linfo ("Copy this into the interface on autogentoo.org to register this server.");
	}
	
	if (!server->sudo_token) {
		server->sudo_token = malloc(AUTOGENTOO_TOKEN_LENGTH + 1);
		prv_random_string(server->sudo_token, AUTOGENTOO_TOKEN_LENGTH);
	}
	
	write_server(server);
	
	while (server->keep_alive) { // Main accept loop
		int temp_fd = accept4(server->socket, (struct sockaddr*) &clientaddr, &addrlen, SOCK_CLOEXEC);
		Connection* current_conn = accept_conn(server, temp_fd, COM_PLAIN);
		
		if (!current_conn) {
			lerror ("Failed to create connection");
			continue;
		}
#ifndef AUTOGENTOO_NO_THREADS
		pool_handler_add(server->pool_handler, (void (*)(void*, int))server_respond, current_conn);
#else
		server_respond (current_conn);
#endif
	}
}

void server_encrypt_start(EncryptServer* server) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	
	
	server->socket = server_init(9491);
	signal (SIGINT, kill_encrypt_server);
	
	linfo("Encrypted server started on port %s", server->port);
	
	addrlen = sizeof(clientaddr);
	
	if (!SSL_CTX_check_private_key(server->context)) {
		lerror("Private key not loaded");
		pthread_kill (pthread_self(), SIGINT);
	}
	
	while (server->parent->keep_alive) { // Main accept loop
		int temp_fd = accept4(server->socket, (struct sockaddr*) &clientaddr, &addrlen, SOCK_CLOEXEC);
		Connection* current_conn = accept_conn(server, temp_fd, COM_RSA);
		
		if (!current_conn) {
			current_conn = accept_conn(server, temp_fd, COM_PLAIN);
			rsend(current_conn, UPGRADE_REQUIRED);
			connection_free(current_conn);
			continue;
		}

#ifndef AUTOGENTOO_NO_THREADS
		pool_handler_add(server->parent->pool_handler, (void (*)(void*, int))server_respond, current_conn);
#else
		server_respond (current_conn);
#endif
	}
	
	pthread_exit (NULL);
}
