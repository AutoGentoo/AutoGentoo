//
// Created by atuser on 11/20/18.
//

#include <autogentoo/server.h>
#include <autogentoo/writeconfig.h>


void handle_sigint (int sig) {
	srv->keep_alive = 0;
	server_kill (srv);
}

void server_kill (Server* server) {
	write_server(server);
	
	if (server->opts & ENCRYPT) {
		pthread_kill(server->rsa_child->pid, SIGINT);
		close(server->rsa_child->socket);
		pthread_join(server->rsa_child->pid, NULL);
		
		server_encrypt_free(server->rsa_child);
		linfo("encrypt server exited");
	}
	pool_exit(server->pool_handler);
	server_free(server);
	close(server->socket);
	linfo("server exited succuessfully");
	fflush(stdout);
	exit (0);
}