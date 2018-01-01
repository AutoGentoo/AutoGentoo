#ifndef __AUTOGENTOO_HOST_H__
#define __AUTOGENTOO_HOST_H__

typedef struct __Host Host;
typedef char* host_id;

#include "response.h"
#include "server.h"

/**
 * @brief Info regarding bit-size of the architecture
 * 
 * This is important because it will decide which directory to
 * link /lib and /usr/lib to
 * 
 * This is marked for removed after HostTemplate API is finished
 */
typedef enum {
    _32BIT,
    _64BIT,
    _INVALIDBIT //!< Could not be determined
} arch_t;


/**
 * @brief Status of the Host
 * 
 * This is marked for removed after HostTemplate API is finished
 */
typedef enum {
    NOT_INITED,
    INIT,
    STAGE1,
    READY
} host_t;

/**
 * @brief Are the chroot directories mounted
 * 
 * This will decide whether or not the system
 * is able to perform a chroot()
 * 
 * This is reset/updated after every restart (not persistant)
 */
typedef enum {
    CHR_NOT_MOUNTED,
    CHR_MOUNTED
} chroot_t;

struct __Host {
    Server* parent; //!< The parent server
    host_id id; //!< The ID of the Host
    char* profile; //!< Portage profile, see possible values with eselect profile list
    char* hostname; //!< Hostname of the host (shows up in the graphical client)
    host_t status; //!< Are we ready to install packages?
    chroot_t chroot_status; //!< Is the chroot ready?
    
    struct {
        char* cflags; //!< The gcc passed to C programs, try -march=native :)
        char* cxxflags; //!< The gcc passed only to CXX programs
        char* arch; //!<  The portage-arch (eg. amd64)
        char* chost; //!< The system chost (should not be changed after it is set)
        char* use; //!< use flags
        StringVector* extra; //!< A list of extra entries to go into make.conf
    } makeconf; //!< These will go in make.conf
    
    struct {
        char* portage_tmpdir; //!< build dir
        char* portdir; //!< ebuild portage tree
        char* distdir; //!< distfiles
        char* pkgdir; //!< path to binaries
        char* port_logdir; //!< logs
    } binhost; //!< Information about the binhost (all values are relative to the Host's root)
    
    //Kernel* kernel;
};

/**
 * Returns a randomly generated HOST_ID
 * @return a new host id
 */
host_id host_id_new ();

/**
 * Creates a new host given an ID
 * @param server the parent server
 * @param id the ID of the Host
 * @return a pointer to the Host
 */
Host* host_new (Server* server, host_id id);

/**
 * Updates the current status of the host by reading its directory
 * @param host the host to update the status of
 */
void host_set_status (Host* host);

/**
 * Writes the path of the host to dest
 * @param host the host to read path from
 * @param dest the destination of the Host's path
 */
void host_get_path (Host* host, char* dest);

/**
 * Free a host
 * @param host the host to free 
 */
void host_free (Host* host);

/**
 * Determine arch (32 or 64bit) from chost
 * @param chost the chost to read
 * @return the determined arch
 */
arch_t determine_arch (char* chost);

/**
 * Write the make.conf (also updates it)
 * @param host the host of which to write the make.conf for
 * @return 0 if successful, 1 if not successful
 */
int host_write_make_conf (Host* host);

/* Request calls */
/**
 * Initilize the host by creating its directory structure \
 * This has a scheduled deprecation due to HostTemplate implementations
 * @param host the host to init
 * @return OK if successful, INTERNAL_ERROR if not
 */
response_t host_init (Host* host);

/**
 * Emerges packages without chrooting (DANGEROUS)
 * 
 * This has a scheduled deprecation due to HostTemplate implementations
 * @param host 
 * @return OK if successful, INTERNAL_ERROR if not
 */
response_t host_stage3 (Host* host);

/**
 * Safely install packages in the chroot
 * @param host the host to install packages for
 * @param arg the arguments passed to emerge
 * @return OK if successful, INTERNAL_ERROR if not
 */
response_t host_install (Host* host, char* arg);

#endif