//
// Created by atuser on 2/11/20.
//

#include <autogentoo/subsystem/namespace.h>
#include <sys/socket.h>
#include <autogentoo/host.h>
#include <stdlib.h>
#include <sys/un.h>
#include <errno.h>
#include <autogentoo/crypt.h>
#include <autogentoo/writeconfig.h>

int namespace_get_flags() {
	return CLONE_NEWUSER | CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNET | CLONE_NEWPID | CLONE_NEWNS;
}

pid_t namespace_spawn(HostNamespace* target, namespace_callback callback, void* arg) {

}

int namespace_main(char* public_key) {
	
	
	return 0;
}

NamespaceManager* ns_manager_new(Server* parent) {
	NamespaceManager* out = malloc(sizeof(NamespaceManager));
	
	out->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	out->host_to_ns = small_map_new(5);
	char* rand = host_id_new();
	
	asprintf(&out->sock_path, "/tmp/%s", rand);
	free(rand);
	
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, out->sock_path, sizeof(addr.sun_path)-1);
	
	if (bind(out->sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
		lerror("bind() failed for ns_manager");
		lerror("Error [%d] %s", errno, strerror(errno));
		
		small_map_free(out->host_to_ns, 0);
		close(out->sock);
		free(out->sock_path);
		free(out);
		
		return NULL;
	}
	
	if (listen(out->sock, 32) != 0) {
		lerror("listen() failed for ns_manager");
		lerror("Error [%d] %s", errno, strerror(errno));
		
		small_map_free(out->host_to_ns,  0);
		close(out->sock);
		free(out->sock_path);
		free(out);
		
		return NULL;
	}
	
	if (!rsa_generate(&out->key)) {
		lerror("Failed to generate namespace encryption key");
		
		small_map_free(out->host_to_ns, 0);
		close(out->sock);
		free(out->sock_path);
		free(out);
		
		return NULL;
	}
	
	return out;
}

void ns_manager_main(NamespaceManager* nsm) {
	while (nsm->keep_alive) {
		struct sockaddr addr;
		socklen_t addr_len;
		
		int client_sock = accept(nsm->sock, &addr, &addr_len);
		if (client_sock < 0)
			continue; /* Failed to accept, could be an nsm->kill() */
		
		ns_command cmd;
		read(client_sock, &cmd, sizeof(cmd));
		
		int len_back = 0;
		
		if (cmd != NS_UPDATE) {
			write(client_sock, &len_back, sizeof(len_back));
			continue;
		}
		
		int ident_len = 0;
		read(client_sock, &ident_len, sizeof(int));
		
		char* host_ident = malloc(sizeof(ident_len));
		read(client_sock, host_ident, ident_len);
		
		Host* target = server_get_host(nsm->parent, host_ident);
		if (!target) {
			free(host_ident);
			write(client_sock, &len_back, sizeof(len_back));
			continue;
		}
		
		void* host_serial = malloc(1024);
		FILE* fp = fmemopen(host_ident, 1024, "w+");
		
		len_back = (int)write_host_fp (target, fp);
		
		write(client_sock, &len_back, sizeof(len_back));
		write(client_sock, host_serial, len_back);
		
		fclose(fp);
		free(host_serial);
	}
}

int ns_manager_start(NamespaceManager* nsm) {
	pthread_create(&nsm->pthread_pid, NULL, (void* (*)(void*)) ns_manager_main, nsm);
	
	
}