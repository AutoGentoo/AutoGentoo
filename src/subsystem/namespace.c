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
#include <err.h>

int namespace_get_flags() {
    return CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS;
    //CLONE_NEWUSER | CLONE_NEWNET | // Don't isolate network and user for now.
}

static int pivot_root(const char* new_root, const char* put_old) {
    return (int) syscall(SYS_pivot_root, new_root, put_old);
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

    if (mount("none", "/", NULL, UNSHARE_PROPAGATION_DEFAULT, NULL) != 0)
        err(EXIT_FAILURE, "cannot change root filesystem propagation");

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
    out = mkdir("/usr", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    out = mkdir("/usr/portage", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    out = mount(ns->worker_dir, "autogentoo/worker", "", MS_BIND | MS_RDONLY, NULL);
    if (out != 0) {
        lerror("ERROR [%d] Failed to mount worker: %s", errno, strerror(errno));
        return out;
    }

    out = mount(ns->portdir, "usr/portage", "", MS_BIND | MS_RDONLY, NULL);
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

    out = umount2("old-root", MNT_DETACH);
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
    return res;
}

struct __prv_stage3_request {
    Host* host;
    char* args;
};

int prv_stage3_wait_thread(struct __prv_stage3_request* stage3_request) {
#ifdef AUTOGENTOO_DEBUG
    char* script = AUTOGENTOO_WORKER_DIR_DEBUG "/stage3.py";
#else
    char* script = AUTOGENTOO_WORKER_DIR "/stage3.py";
#endif

    int ret = 0;
    char* argv[] = {script, stage3_request->host->parent->location, stage3_request->host->id, stage3_request->args,
                    NULL};
    pid_t pid = fork();
    if (pid == -1) {
        lerror("fork() failed [%d]: %s", errno, strerror(errno));
        exit(errno);
    } else if (pid == 0) {
        int res = execv(script, argv);
        exit(res);
    } else {
        host_set_chroot(stage3_request->host, CHR_INIT);
        waitpid(pid, &ret, 0);
        Namespace* ns = NULL;
        if (ret == 0) { /* Stage3 init successful */
            ns = ns_new(stage3_request->host);
        }
        if (ret != 0 || !ns) {
            host_set_chroot(stage3_request->host, CHR_NOT_MOUNTED);
            lerror("Failed to init stage3 for %s", stage3_request->host->id);
            errno = ret;
        } else {
            linfo("Successful stage3 of %s", stage3_request->host->id);
            small_map_insert(stage3_request->host->parent->job_handler->host_to_ns, stage3_request->host->id, ns);
        }

        free(stage3_request->args);
    }

    return ret;
}

int stage3_bootstrap(Host* host, const char* args) {
    struct __prv_stage3_request* stage3_request = malloc(sizeof(struct __prv_stage3_request));

    stage3_request->host = host;
    stage3_request->args = strdup(args);

    pthread_t wait_thread;
    pthread_create(&wait_thread, NULL, (void* (*)(void*)) prv_stage3_wait_thread, stage3_request);

    return 0;
}

Namespace* ns_new(Host* target) {
    struct stat stat_buf;
    char* stage3_check = host_path(target, "/.stage3");
    if (stat(stage3_check, &stat_buf) != 0) {
        host_set_chroot(target, CHR_NOT_MOUNTED);
        free(stage3_check);
        return NULL;
    }

    free(stage3_check);

    Namespace* out = malloc(sizeof(Namespace));

    out->target = target;
    out->target_dir = host_path(target, "");
    out->portdir = "/usr/portage";

#ifdef AUTOGENTOO_DEBUG
    out->worker_dir = AUTOGENTOO_WORKER_DIR_DEBUG;
#else
    out->worker_dir = AUTOGENTOO_WORKER_DIR;
#endif

    out->worker_pid = -1;
    out->running = 1;

    if (clone((int (*)(void*)) namespace_main, malloc(1024 * 1024), namespace_get_flags(), out) == -1) {
        lerror("Failed to start namespace worker %s", target->id);
        lerror("error [%d] %s", errno, strerror(errno));
        exit(errno);
    }

    host_set_chroot(target, CHR_MOUNTED);

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
    Namespace* found = small_map_get(job_request->parent->host_to_ns, job_request->target->id);

    if (!found) {
        found = ns_new(job_request->target);
        small_map_insert(job_request->parent->host_to_ns, job_request->target->id, found);
    }

    /* Taken from https://github.com/troydhanson/network */
    int fd;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        return NULL;
    }

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        return NULL;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    char* socket_path = host_path(job_request->target, "/autogentoo/comm.uds");
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    size_t ptr_buf_n = 0;
    char* ptr_buf = NULL;
    FILE* buff_sock = open_memstream(&ptr_buf, &ptr_buf_n);
    if (!buff_sock) {
        perror("open_memstream() failed");
        close(fd);
    }
    write_host_fp(job_request->target, buff_sock);
    write_string(job_request->script, buff_sock);

    write_string(job_request->arg, buff_sock);

    fflush(buff_sock);
    long pos = ftell(buff_sock);

    if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("connect error");
        close(fd);

        return NULL;
    }

    int wrote = write(fd, ptr_buf, (int) pos);
    if (wrote == -1) {
        perror("write() error");
        close(fd);

        return NULL;
    }

    fclose(buff_sock);
    free(ptr_buf);

    *res = read_int_fd(fd);
    char* job_name = read_string_fd(fd);

    close(fd);

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