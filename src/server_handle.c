//
// Created by atuser on 11/20/18.
//

#include <autogentoo/server.h>
#include <autogentoo/writeconfig.h>
#include <errno.h>

void handle_sigint (int sig) {
	srv->keep_alive = 0;
	server_kill (srv);
}

void kill_encrypt_server(int sig) {
	int ret = 0;
	server_kill(srv);
	exit(0);
}

void server_kill (Server* server) {
	write_server(server);
	
	if (server->opts & ENCRYPT) {
		pthread_kill(server->rsa_child->pid, SIGINT);
		close(server->rsa_child->socket);
		pthread_join(server->rsa_child->pid, NULL);
		
		worker_handler_free(server->job_handler);
		
		server_encrypt_free(server->rsa_child);
		linfo("encrypt server exited");
	}
	pool_exit(server->pool_handler);
	close(server->socket);
	server_free(server);
	linfo("server exited succuessfully");
	exit (0);
}