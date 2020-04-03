//
// Created by atuser on 2/11/20.
//

#ifndef AUTOGENTOO_NAMESPACE_H
#define AUTOGENTOO_NAMESPACE_H

#define _GNU_SOURCE

typedef struct __Namespace Namespace;
typedef struct __NamespaceManager NamespaceManager;
typedef struct __Job Job;

/* From util-linux */
#define _PATH_PROC_UIDMAP	"/proc/self/uid_map"
#define _PATH_PROC_GIDMAP	"/proc/self/gid_map"
#define _PATH_PROC_SETGROUPS	"/proc/self/setgroups"

#include <fcntl.h>
#include <sched.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include <openssl/rsa.h>
#include <autogentoo/host.h>

typedef int (*namespace_callback)(void* arg);

typedef enum {
	/* Server to worker */
	NS_WORK,
	NS_HOST_REPL,
	NS_KILL,
	
	/* Worker to server */
	NS_UPDATE
} ns_command;

extern struct namespace_file {
	int nstype;
	const char *name;
	int fd;
} namespace_files[];

struct __Namespace {
	pid_t worker_pid;
	Host* target;
	
	char* portdir; /* Gentoo package repo (read-only) */
	char* target_dir; /* Where we want to chroot */
	char* worker_dir; /* AutoGentoo worker scripts (read-only) */
	
	int running;
};

struct __NamespaceManager {
	Server* parent;
	SmallMap* host_to_ns;
};

struct __Job {
	NamespaceManager* parent;
	Host* target;
	
	char* script;
	
	int argc;
	char** argv;
};

int namespace_get_flags();
int namespace_main(Namespace* ns);

Namespace* ns_new(Host* target);
NamespaceManager* nsm_new(Server* parent);
void nsm_free(NamespaceManager* nsm);

/**
 * Request a worker to perform a job
 * @param job_request the job holding the necessary information
 * @return the job name as a string
 */
char* nsm_job(Job* job_request, int* res);

#endif //AUTOGENTOO_NAMESPACE_H
