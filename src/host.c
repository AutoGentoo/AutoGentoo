#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <host.h>
#include <time.h>

host_id host_id_new () {
    int len = 15;
    host_id out = malloc (len);
    
    srandom(time(NULL));  // Correct seeding function for random()
    char c;
    int i;
    for (i=0; i != len - 1; i++) {
        c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[random () % 62];
        out[i] = c;
    }
    out[len] = 0;
    
    return out;
}

Host* host_new (Server* server, host_id id) {
    Host* out = malloc (sizeof (Host));
    out->parent = server;
    out->id = id; // Dont need to dup, never accessed elsewhere
    
    out->id = NULL;
    out->profile = NULL;
    out->makeconf.cflags = NULL;
    out->makeconf.cxxflags = NULL;
    out->makeconf.use = NULL;
    out->makeconf.chost = NULL;
    out->makeconf.extra = NULL;
    out->chroot_info.portage_dir = NULL;
    out->chroot_info.resolv_conf = NULL;
    out->chroot_info.locale = NULL;
    out->binhost.portage_tmpdir = NULL;
    out->binhost.portdir = NULL;
    out->binhost.distdir = NULL;
    out->binhost.pkgdir = NULL;
    out->binhost.port_logdir = NULL;
    
    return out;
}

void host_set_status (Server* server, Host* host);
void host_free (Host* host) {
    free(host->id);
    free(host->profile);
    free(host->makeconf.cflags);
    free(host->makeconf.cxxflags);
    free(host->makeconf.use);
    free(host->makeconf.chost);
    string_vector_free (host->makeconf.extra);
    free(host->chroot_info.portage_dir);
    free(host->chroot_info.resolv_conf);
    free(host->chroot_info.locale);
    free(host->binhost.portage_tmpdir);
    free(host->binhost.portdir);
    free(host->binhost.distdir);
    free(host->binhost.pkgdir);
    free(host->binhost.port_logdir);
    free (host);
}

/* System calls */
void host_write_make_conf (Host* host);
void host_write (Host* host, int fd); // Write to file fd
void host_read (Host* host, int fd); // Read from file fd

/* Request calls */
response_t host_init (Host* host);
response_t host_stage1_install (Host* host);
response_t host_install (Host* host, char* arg);