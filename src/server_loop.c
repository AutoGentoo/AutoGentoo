//
// Created by atuser on 11/20/18.
//

#include <autogentoo/server.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/ssl.h>

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
	return out;
}

void server_start (Server* server) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	server->pthread = pthread_self();
	server->socket = server_init(server->port);
	
	linfo("Server started on port %s", server->port);
	
	if (server->opts & DAEMON)
		daemonize(server->location);
	
	server->pid = getpid();
	srv = server;
	signal (SIGINT, handle_sigint);
	
	addrlen = sizeof(clientaddr);
	server->keep_alive = 1;
	//server->thandler = thread_handler_new(32);
#ifndef AUTOGENTOO_NO_THREADS
	server->pool_handler = pool_handler_new(32);
#endif
	
	while (server->keep_alive) { // Main accept loop
		int temp_fd = accept(server->socket, (struct sockaddr*) &clientaddr, &addrlen);
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
	
	server->socket = server_init(server->port);
	signal (SIGINT, kill_encrypt_server);
	
	linfo("Encrypted server started on port %s", server->port);
	
	addrlen = sizeof(clientaddr);
	
	if (!SSL_CTX_check_private_key(server->context)) {
		lerror("Private key not loaded");
		pthread_kill (pthread_self(), SIGINT);
	}
	
	while (server->parent->keep_alive) { // Main accept loop
		int temp_fd = accept(server->socket, (struct sockaddr*) &clientaddr, &addrlen);
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
