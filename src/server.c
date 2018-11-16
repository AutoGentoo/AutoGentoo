#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <autogentoo/autogentoo.h>
#include <signal.h>
#include <autogentoo/crypt.h>

static Server* srv = NULL;

void handle_thread_kill (int signum) {
	thread_join(srv->thandler, srv->thandler->to_join);
	if (srv->keep_alive == 0) {
		server_kill(srv);
		exit(0);
	}
}

Server* server_new (char* location, char* port, server_t opts) {
	Server* out = malloc(sizeof(Server));
	
	out->templates = vector_new(sizeof(HostTemplate*), REMOVE | UNORDERED);
	out->hosts = vector_new(sizeof(Host*), REMOVE | UNORDERED);
	out->stages = small_map_new(sizeof(HostTemplate*), 5);
	out->host_bindings = vector_new(sizeof(HostBind), REMOVE | UNORDERED);
	out->location = strdup(location);
	out->queue = malloc (sizeof (WorkerParent));
	out->queue->tail = NULL;
	out->queue->head = NULL;
	out->queue->proc_id = -1;
	
	chdir(out->location);
	out->opts = opts;
	out->port = strdup(port);
	
	if (out->opts & ENCRYPT)
		out->rsa_child = server_encrypt_new(out, "4950");
	
	return out;
}

EncryptServer* server_encrypt_new (Server* parent, char* port) {
	EncryptServer* out = malloc (sizeof (EncryptServer));
	
	out->parent = parent;
	out->port = strdup (port);
	out->rsa_binding = small_map_new(16, 16);
	
	if (rsa_generate(parent) != 0) {
		fprintf (stderr, "Failed to generate private key\n");
		server_free(parent);
		exit (1);
	}
	
	return out;
}

Host* server_get_active (Server* server, char* ip) {
	int i;
	for (i = 0; i != server->host_bindings->n; i++) {
		HostBind* current_bind = (HostBind*) vector_get(server->host_bindings, i);
		if (strcmp(ip, current_bind->ip) == 0)
			return current_bind->host;
	}
	
	return NULL;
}

HostBind* prv_server_get_active_location (Server* server, char* ip) {
	int i;
	for (i = 0; i != server->host_bindings->n; i++) {
		HostBind* current_bind = (HostBind*) vector_get(server->host_bindings, i);
		if (strcmp(ip, current_bind->ip) == 0)
			return current_bind;
	}
	
	return NULL;
}

Connection* connection_new (Server* server, int conn_fd) {
	Connection* out = malloc(sizeof(Connection));
	out->parent = server;
	out->fd = conn_fd;
	
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	getpeername(out->fd, (struct sockaddr*) &addr, &addr_size);
	out->ip = strdup(inet_ntoa(addr.sin_addr));
	out->bounded_host = server_get_active(out->parent, out->ip);
	
	return out;
}

void server_start (Server* server) {
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	server->pthread = pthread_self();
	
	int listenfd = server_init(server->port);
	
	linfo("Server started on port %s", server->port);
	
	if (server->opts & DAEMON)
		daemonize(server->location);
	
	server->pid = getpid();
	srv = server;
	signal(SIGUSR1, handle_thread_kill);
	signal (SIGINT, handle_sigint);
	
	addrlen = sizeof(clientaddr);
	server->keep_alive = 1;
	server->thandler = thread_handler_new(32);
	
	while (server->keep_alive) { // Main accept loop
		int temp_fd = accept(listenfd, (struct sockaddr*) &clientaddr, &addrlen);
		if (temp_fd < 3) {
			lwarning("accept() error");
			continue;
		}
		if (fcntl(temp_fd, F_GETFD) == -1 || errno == EBADF) {
			lwarning("Bad fd on accept()");
			fflush(stdout);
			continue;
		}
		Connection* current_conn = connection_new(server, temp_fd);
		current_conn->communication_type = COM_PLAIN;

#ifndef AUTOGENTOO_NO_THREADS
		pthread_t p_pid;
		
		if (pthread_create(&p_pid, NULL, (void* (*) (void*)) server_respond, current_conn)) {
			lerror("Error creating thread");
			fflush(stdout);
			exit(1);
		}
#else
		server_respond (current_conn);
#endif
	}
}

pid_t server_encrypt_start(EncryptServer* server) {
	pid_t child_pid = fork();
	if (child_pid)
		return child_pid;
	
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	
	int listenfd = server_init(server->port);
	
	linfo("Encrypted server started on port %s", server->port);
	
	addrlen = sizeof(clientaddr);
	
	while (server->parent->keep_alive) { // Main accept loop
		int temp_fd = accept(listenfd, (struct sockaddr*) &clientaddr, &addrlen);
		if (temp_fd < 3) {
			lwarning("accept() error");
			continue;
		}
		if (fcntl(temp_fd, F_GETFD) == -1 || errno == EBADF) {
			lwarning("Bad fd on accept()");
			fflush(stdout);
			continue;
		}
		Connection* current_conn = connection_new(server->parent, temp_fd);
		current_conn->communication_type = COM_RSA;
		
		if (rsa_perform_handshake(current_conn) != 0) {
			fprintf (stderr, "Failed to perform rsa handshake\n");
			connection_free(current_conn);
			continue;
		}
		
		current_conn->encrypted_fd = BIO_new(BIO_s_fd());
		PEM_write_bio_RSAPublicKey(current_conn->encrypted_fd, current_conn->public_key);
		PEM_write_bio_RSAPrivateKey(current_conn->encrypted_fd, server->private_key, NULL, NULL, 0, NULL, NULL);

#ifndef AUTOGENTOO_NO_THREADS
		pthread_t p_pid;
		
		if (pthread_create(&p_pid, NULL, (void* (*) (void*)) server_respond, current_conn)) {
			lerror("Error creating thread");
			fflush(stdout);
			exit(1);
		}
#else
		server_respond (current_conn);
#endif
	}
	
	exit (0);
}

void server_kill (Server* server) {
	write_server(server);
	thread_handler_join_all(server->thandler);
	thread_handler_free(server->thandler);
	server_free(server);
	linfo("server exited succuessfully");
	exit (0);
}

int server_init (char* port) {
	struct addrinfo hints, * res, * p;
	int listenfd = -1;
	
	// getaddrinfo for host
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &res) != 0) {
		lerror("getaddrinfo() error");
		exit(1);
	}
	// socket and bind
	for (p = res; p != NULL; p = p->ai_next) {
		listenfd = socket(p->ai_family, p->ai_socktype, 0);
		if (listenfd == -1)
			continue;
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
			lerror("setsockopt(SO_REUSEADDR) failed");
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
	}
	if (p == NULL) {
		lerror("socket() or bind()");
		switch (errno) {
			case EADDRINUSE:
				lerror("Port %s in use", port);
				break;
			case EACCES:
				lerror("Permission denied");
				break;
			case ENETUNREACH:
				lerror("Network unreachable");
				break;
			default:
				break;
		}
		exit(1);
	}
	
	freeaddrinfo(res);
	
	// listen for incoming connections
	
	if (listen(listenfd, 64) != 0) {
		lerror("listen() error");
		exit(1);
	}
	
	return listenfd;
}

void connection_free (Connection* conn) {
	if (conn->status != CLOSED) { ;
		if (fcntl(conn->fd, F_GETFD) != -1 || errno != EBADF)
			close(conn->fd);
		conn->status = CLOSED;
	}
	
	if (conn->communication_type == COM_RSA) {
		BIO_free_all(conn->encrypted_fd);
		RSA_free(conn->public_key);
	}
	
	free(conn->request);
	free(conn->ip);
	free(conn);
}

void handle_sigint (int sig) {
	srv->keep_alive = 0;
	server_kill (srv);
}

void handle_segv (int signum) {
	Connection* failed_thread = thread_get_conn(srv->thandler, pthread_self());
	rsend(failed_thread, AUTOGENTOO_SEGV);
	
	failed_thread->parent->thandler->to_join = failed_thread->pid;
	connection_free(failed_thread);
#ifndef AUTOGENTOO_NO_THREADS
	pthread_kill(failed_thread->parent->pthread, SIGUSR1);
#endif
}
//#define AUTOGENTOO_IGNORE_SEGV

void server_recv (Connection* conn) {
	/* Read the request */
	size_t chunk_len = 128;
	
	conn->request = malloc(chunk_len);
	ssize_t total_read = 0, current_bytes = 0;
	
	size_t buffer_size = chunk_len;
	current_bytes = read(conn->fd, conn->request, chunk_len);
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
		lerror("recv() error");
		conn->status = SERVER_ERROR;
		connection_free(conn);
		pthread_kill(conn->pid, SIGUSR1);
		return;
	} else if (total_read == 0) { // receive socket closed
		lwarning("Client disconnected upexpectedly.");
		conn->status = FAILED;
		connection_free(conn);
		pthread_kill(conn->pid, SIGUSR1);
		return;
	}
	
	conn->size = (size_t) total_read;
	conn->status = CONNECTED;
}

void server_respond (Connection* conn) {
#ifndef AUTOGENTOO_NO_THREADS
	conn->pid = pthread_self();
	thread_register(conn->parent->thandler, conn->pid, conn);
#else
	conn->pid = 0;
#endif
	
	server_recv(conn);
	
	response_t res;
	Request* request = request_handle(conn);
	
	linfo("handle %s on pthread_t 0x%llx", conn->ip, conn->pid);
	if (request == NULL)
		res = BAD_REQUEST;
	else
		res = request_call(request);
	if (res.len != 0)
		rsend(conn, res);
	linfo("request 0x%llx: %s (%d)", conn->pid, res.message, res.code);
	if (conn->parent) // This went NULL once, won't take chances
		write_server(conn->parent);
	srv->thandler->to_join = pthread_self();
	pthread_t parent = srv->pthread;
	connection_free(conn);
	if (request)
		request_free(request);

#ifndef AUTOGENTOO_NO_THREADS
	pthread_kill(parent, SIGUSR1);
#endif
}

void server_bind (Connection* conn, Host* host) {
	HostBind* loc = prv_server_get_active_location(conn->parent, conn->ip);
	if (loc == NULL) {
		HostBind new_binding = {.ip = strdup(conn->ip), .host = host};
		vector_add(conn->parent->host_bindings, &new_binding);
	} else
		loc->host = host;
}

Host* server_host_search (Server* server, char* id) {
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
	if (pid < 0) {
		exit(1);
	}
	
	if (pid > 0) {
		linfo("Forked to pid: %d", (int) pid);
		linfo("Moving to background");
		fflush(stdout);
		exit(0); /*Killing the Parent Process*/
	}
	
	/* At this point we are executing as the child process */
	
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(1);
	}
	
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
	
	for (i = 0; i != server->host_bindings->n; i++)
		free((*(HostBind*) vector_get(server->host_bindings, i)).ip);
	
	small_map_free(server->stages, 0);
	vector_free(server->hosts);
	vector_free(server->host_bindings);
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