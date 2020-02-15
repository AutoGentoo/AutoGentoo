//
// Created by atuser on 2/11/20.
//

#ifndef AUTOGENTOO_NAMESPACE_H
#define AUTOGENTOO_NAMESPACE_H

#define _GNU_SOURCE

#include <fcntl.h>
#include <sched.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include <openssl/rsa.h>
#include <autogentoo/host.h>

typedef int (*namespace_callback)(void* arg);

typedef struct __HostNamespace HostNamespace;
typedef struct __NamespaceManager NamespaceManager;
typedef struct __Namespace Namespace;

typedef enum {
	/* Server to worker */
	NS_WORK,
	NS_HOST_REPL,
	NS_KILL,
	
	/* Worker to server */
	NS_UPDATE
} ns_command;

static struct namespace_file {
	int nstype;
	const char *name;
	int fd;
} namespace_files[] = {
		/* Careful the order is significant in this array.
		 *
		 * The user namespace comes either first or last: first if
		 * you're using it to increase your privilege and last if
		 * you're using it to decrease.  We enter the namespaces in
		 * two passes starting initially from offset 1 and then offset
		 * 0 if that fails.
		 */
		{ .nstype = CLONE_NEWUSER,  .name = "ns/user", .fd = -1 },
		{ .nstype = CLONE_NEWCGROUP,.name = "ns/cgroup", .fd = -1 },
		{ .nstype = CLONE_NEWIPC,   .name = "ns/ipc",  .fd = -1 },
		{ .nstype = CLONE_NEWUTS,   .name = "ns/uts",  .fd = -1 },
		{ .nstype = CLONE_NEWNET,   .name = "ns/net",  .fd = -1 },
		{ .nstype = CLONE_NEWPID,   .name = "ns/pid",  .fd = -1 },
		{ .nstype = CLONE_NEWNS,    .name = "ns/mnt",  .fd = -1 },
		{ .nstype = 0, .name = NULL, .fd = -1 }
};

struct __HostNamespace {
	Host* parent;
	pid_t init_pid;
	int ns_flags;
	
	/* public key send from init script */
	RSA* key;
};

struct __Namespace {
	Host* target;
	
	char* portdir; /* Gentoo package repo (read-only) */
	char* target_dir; /* Where we want to chroot */
	char* worker_dir; /* AutoGentoo worker scripts (read-only) */
};

struct __NamespaceManager {
	Server* parent;
	SmallMap* host_to_ns;
	
	int sock;
	char* sock_path;
	
	/* Private key generated on server
	 * No public key here
	*/
	RSA* key;
	
	int keep_alive;
	pthread_t pthread_pid;
	pthread_mutex_t init_lock;
};

int namespace_get_flags();
int namespace_main(Namespace* ns);
HostNamespace* namespace_new(Host* parent);
NamespaceManager* ns_manager_new(Server* parent);
int ns_manager_start(NamespaceManager* nsm);

#endif //AUTOGENTOO_NAMESPACE_H
