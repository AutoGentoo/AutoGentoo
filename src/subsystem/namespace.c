//
// Created by atuser on 2/11/20.
//

#include <autogentoo/subsystem/namespace.h>
#include <autogentoo/host.h>
#include <stdlib.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <syscall.h>
#include <autogentoo/writeconfig.h>
#include <wait.h>
#include <libiberty.h>
#include <err.h>

struct namespace_file namespace_files[] = {
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

enum {
    SETGROUPS_NONE = -1,
    SETGROUPS_DENY = 0,
    SETGROUPS_ALLOW = 1,
};

static const char *setgroups_strings[] = {
    [SETGROUPS_DENY] = "deny",
    [SETGROUPS_ALLOW] = "allow"
};

int namespace_get_flags() {
	return CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS;
	//CLONE_NEWUSER | CLONE_NEWNET | // Don't isolate network and user for now.
}

static int pivot_root(const char *new_root, const char *put_old) {
	return (int)syscall(SYS_pivot_root, new_root, put_old);
}

/* Taken from https://github.com/karelzak/util-linux/blob/master/sys-utils/unshare.c */
static void map_id(const char *file, uint32_t from, uint32_t to) {
    char *buf;
    int fd;

    fd = open(file, O_WRONLY);
    if (fd < 0) {
        lerror("cannot open file: %s", file);
        exit(errno);
    }

    asprintf(&buf, "%u %u 1", from, to);

    size_t len = strlen(buf);
    if (write(fd, buf, len) == 0) {
        err(EXIT_FAILURE, "write failed %s", file);
    }

    free(buf);
    close(fd);
}

static void setgroups_control(int action) {
    const char *file = _PATH_PROC_SETGROUPS;
    const char *cmd;
    int fd;

    if (action < 0 || (size_t) action >= ARRAY_SIZE(setgroups_strings))
        return;
    cmd = setgroups_strings[action];

    fd = open(file, O_WRONLY);
    if (fd < 0) {
        if (errno == ENOENT)
            return;
        err(EXIT_FAILURE, "cannot open %s", file);
    }

    if (write(fd, cmd, strlen(cmd)) == 0)
        err(EXIT_FAILURE, "write failed %s", file);
    close(fd);
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

	/*
    uid_t real_euid = geteuid();
    gid_t real_egid = getegid();

    setgroups_control(SETGROUPS_DENY);
    map_id(_PATH_PROC_UIDMAP, 0, real_euid);
    map_id(_PATH_PROC_GIDMAP, 0, real_egid);

    seteuid(0);
    setegid(0);*/

	linfo("Chroot protocol with user: %d", geteuid());

	int out = 0;
	
	linfo("mount --bind %s %s", ns->target_dir, ns->target_dir);
	if ((out = mount(ns->target_dir, ns->target_dir, "", MS_BIND, NULL) != 0)) {
		lerror("ERROR [%d] Failed to perform self mount: %s", errno, strerror(errno));
		return out;
	}
	
	out = chdir(ns->target_dir);

    out = mkdir("autogentoo/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	out = mkdir("autogentoo/worker/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	out = mkdir("autogentoo/portage", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	out = mount(ns->worker_dir, "autogentoo/worker", "", MS_BIND | MS_RDONLY, NULL);
    if (out != 0) {
        lerror("ERROR [%d] Failed to mount worker: %s", errno, strerror(errno));
        return out;
    }

	out = mount(ns->portdir, "autogentoo/portage", "", MS_BIND | MS_RDONLY, NULL);
    if (out != 0) {
        lerror("ERROR [%d] Failed to mount portage: %s", errno, strerror(errno));
        return out;
    }

	out = mkdir("old-root", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	out = pivot_root(".", "old-root");
    if (out != 0) {
        lerror("ERROR [%d] Failed to pivot_root: %s", errno, strerror(errno));
        return out;
    }
	
	out = umount2("oldroot", MNT_DETACH);
    if (out != 0) {
        lerror("ERROR [%d] failed to unmount old root: %s", errno, strerror(errno));
        return out;
    }

	out = mount("proc", "proc", "proc", MS_MGC_VAL, NULL);
    if (out != 0) {
        lerror("ERROR [%d] Failed to mount proc: %s", errno, strerror(errno));
        return out;
    }

    out = chroot(".");
	
	out = sethostname(ns->target->hostname, strlen(ns->target->hostname));
    if (out != 0) {
        lerror("ERROR [%d] Failed to set hostname: %s", errno, strerror(errno));
        return out;
    }

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
	
	res = execv(script_name, argv);
	exit(res);

	return res;
}

Namespace* ns_new(Host* target) {
	Namespace* out = malloc(sizeof(Namespace));
	
	out->target = target;
	out->target_dir = host_path(target, "");
	out->portdir = "/usr/portage";   // NEED TO CHANGE !!!!!

#ifdef AUTOGENTOO_DEBUG
	out->worker_dir = AUTOGENTOO_WORKER_DIR_DEBUG;
#else
	out->worker_dir = AUTOGENTOO_WORKER_DIR;
#endif
	
	out->worker_pid = -1;
	out->running = 1;
	out->worker_pid = clone((int (*)(void *)) namespace_main, malloc(4096) + 4096, SIGCHLD | namespace_get_flags(), out);

	if (out->worker_pid == -1) { /* Child thread */
		lerror("Failed to start namespace worker %s", target->id);
		lerror("error [%d] %s", errno, strerror(errno));
        exit(errno);
	}
	
	return out;
}

NamespaceManager* nsm_new(Server* parent) {
	NamespaceManager* out = malloc(sizeof(NamespaceManager));
	
	out->parent = parent;
	out->host_to_ns = small_map_new(parent->hosts->n);
	
	return out;
}

/**
 * Request a worker to perform a job
 * @param job_request the job holding the necessary information
 * @return the job name as a string
 */
char* nsm_job(Job* job_request, int* res) {
	Namespace* found  = small_map_get(job_request->parent->host_to_ns, job_request->target->id);
	
	if (!found) {
		found = ns_new(job_request->target);
		small_map_insert(job_request->parent->host_to_ns, job_request->target->id, found);
	}
	
	/* Taken from https://github.com/troydhanson/network */
	int fd;
	struct sockaddr_un addr;
	
	if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return NULL;
	}
	
	if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return NULL;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	char* socket_path = host_path(job_request->target, "/worker/comm.uds");
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
	
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect error");
		close(fd);
		
		return NULL;
	}
	
	FILE* sock = fdopen(fd, "rb+");
	
	write_host_fp(job_request->target, sock);
	write_string(job_request->script, sock);
	
	write_int(job_request->argc, sock);
	for (int i = 0; i < job_request->argc; i++)
		write_string(job_request->argv[i], sock);
	
	*res = read_int(sock);
	char* job_name = read_string(sock);
	
	fclose(sock);
	
	return job_name;
}

void ns_free(Namespace* ptr) {
	kill(ptr->worker_pid, SIGUSR1);
	
	int res;
	waitpid(ptr->worker_pid, &res, 0);
	
	ptr->running = 0;
	
	free(ptr->worker_dir);
	free(ptr->portdir);
	free(ptr->target_dir);
	
	free(ptr);
}

void nsm_free(NamespaceManager* nsm) {
	for (int i = 0; i < nsm->host_to_ns->n; i++)
		ns_free(small_map_get_index(nsm->host_to_ns, i));
	
	small_map_free(nsm->host_to_ns, 0);
	free(nsm);
}