//
// Created by atuser on 11/20/18.
//

#include <autogentoo/server.h>
#include <autogentoo/writeconfig.h>

void handle_sigusr1(int sig) {
	worker_toggle(srv);
}

void handle_sigusr2(int sig) {
	worker_ack(srv);
}

void handle_sigint (int sig) {
	srv->keep_alive = 0;
	server_kill (srv);
	exit(0);
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
	close(server->socket);
	server_free(server);
	linfo("server exited succuessfully");
	exit (0);
}