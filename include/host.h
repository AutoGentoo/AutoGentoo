#ifndef __AUTOGENTOO_HOST_H__
#define __AUTOGENTOO_HOST_H__

typedef struct __Host Host;
typedef char* host_id;

#include "response.h"
#include "server.h"

typedef enum {
    _32BIT,
    _64BIT,
    _INVALIDBIT
} arch_t;

typedef enum {
    NOT_INITED,
    INIT,
    STAGE1,
    READY
} host_t;

typedef enum {
    CHR_NOT_MOUNTED,
    CHR_MOUNTED
} chroot_t;

struct __Host {
    Server* parent;
    host_id id;
    char* profile;
    char* hostname;
    host_t status;
    chroot_t chroot_status;
    
    // Architecture configuration
    struct {
        char* cflags;
        char* cxxflags;
        char* chost;
        char* use;
        StringVector* extra;
    } makeconf;
    
    struct {
        char* portage_tmpdir; // build dir
        char* portdir; // ebuild portage tree
        char* distdir; // distfiles
        char* pkgdir; // path to binaries
        char* port_logdir; // logs
    } binhost;
    
    //Kernel* kernel;
};

host_id host_id_new ();
Host* host_new (Server* server, host_id id);
void host_set_status (Host* host);
void host_get_path (Host* host, char* dest);
void host_free (Host* host);
arch_t determine_arch (char* chost);

/* System calls */
int host_write_make_conf (Host* host, char* path);
void host_write (Host* host, int fd); // Write to file fd
void host_read (Host* host, int fd); // Read from file fd

/* Request calls */
response_t host_init (Host* host);
response_t host_stage1_install (Host* host, char* arg);
response_t host_install (Host* host, char* arg);

#endif