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
#include <sys/mount.h>
#include <sys/stat.h>
#include <syscall.h>

int namespace_get_flags() {
	return CLONE_NEWUSER | CLONE_NEWIPC | CLONE_NEWUTS |
	//CLONE_NEWNET | // Don't isolate network for now.
	CLONE_NEWPID | CLONE_NEWNS;
}

static int pivot_root(const char *new_root, const char *put_old) {
	return (int)syscall(SYS_pivot_root, new_root, put_old);
}

int namespace_chroot_pivot(Namespace* ns) {
	/*
	 * 1. Remount the chroot dir as a bind
	 *    (pivot needs the root to be mounted)
	 * 2. bind-mount worker ro
	 * 3. bind-mount portage ro
	 * 4. Create an old-root buffer
	 * 5. pivot root to old-root
	 * 6. umount lazy on old-root
	 * 7. mount a procfs
	 * 8. chroot.
	 * */
	
	int out = 0;
	
	out += mount(ns->target_dir, ns->target_dir, "", MS_BIND, NULL);
	out += chdir(ns->target_dir);
	
	out += mkdir("autogentoo/worker/", 0700);
	out += mkdir("autogentoo/portage", 0700);
	
	out += mount(ns->worker_dir, "autogentoo/worker", "", MS_BIND | MS_RDONLY, NULL);
	out += mount(ns->portdir, "autogentoo/portage", "", MS_BIND | MS_RDONLY, NULL);
	
	out += mkdir("old-root", 0700);
	out += pivot_root(".", "old-root");
	
	umount2("oldroot", MNT_DETACH);
	
	out += mount("proc", "proc", "proc", MS_MGC_VAL, NULL);
	out += chroot(".");
	
	out += sethostname(ns->target->hostname, strlen(ns->target->hostname));
	
	return out;
}

int namespace_main(Namespace* ns) {
	int res = namespace_chroot_pivot(ns);
	if (res != 0) {
		lerror("Failed to enter chroot isolation");
		lerror("Error [%d] %s", errno, strerror(errno));
		
		return res;
	}
	
	char* script_name = "/autogentoo/worker/worker.py";
	char* argv[] = {script_name, NULL};
	
	/* Take over this namespace
	 * All processes will be spawned from here
	 * */
	res = execv(script_name, argv);
	return res;
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
	
	pthread_mutex_init(&out->init_lock, NULL);
	pthread_mutex_lock(&out->init_lock);
	
	return out;
}

void ns_manager_main(NamespaceManager* nsm) {
	while (nsm->keep_alive) {
		struct sockaddr addr;
		socklen_t addr_len;
		
		pthread_mutex_unlock(&nsm->init_lock);
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
	
	/* Wait for nsm to initialize and accept connections */
	pthread_mutex_lock(&nsm->init_lock);
	pthread_mutex_unlock(&nsm->init_lock);
}