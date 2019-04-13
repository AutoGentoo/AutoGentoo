#ifndef __AUTOGENTOO_HOST_H__
#define __AUTOGENTOO_HOST_H__

/**
 * @brief Holds information about a build environment
 * 
 * Most of this struct will contain information that is used in
 * /etc/portage/make.conf.
 */
typedef struct __Host Host;

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "response.h"
#include "server.h"
#include "kernel.h"

#define AUTOGETNOO_HOST_ID_LENGTH 15

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

typedef enum {
	HOST_ENV_VOID,   // Environment does not exist
	HOST_ENV_STAGE3, // Stage3 is unpacked but configs need writing
	HOST_ENV_BASE,   // Stage3 packages still need to be made into binaries
	HOST_ENV_READY   // Everything is ready to go
} host_env_t;

/**
 * @brief Holds information about a build environment
 * 
 * Most of this struct will contain information that is used in
 * /etc/portage/make.conf.
 */
struct __Host {
	Server* parent; //!< The parent server
	char* id; //!< The ID of the Host
	char* profile; //!< Portage profile, see possible values with eselect profile list
	char* hostname; //!< Hostname of the host (shows up in the graphical client)
	char* arch; //!<  The portage-arch (eg. amd64)
	chroot_t chroot_status; //!< Is the chroot ready?
	host_env_t environment_status; //!< Is the environment ready to compile
	
	/*
	char* cflags; //!< The gcc passed to C programs, try -march=native :)
	char* cxxflags; //!< The gcc passed only to CXX programs
	char* chost; //!< The system chost (should not be changed after it is set)
	char* use; //!< use flags
	StringVector* extra; //!< A list of extra entries to go into make.conf
	
	char* portage_tmpdir; //!< build dir
	char* portdir; //!< ebuild portage tree
	char* distdir; //!< distfiles
	char* pkgdir; //!< path to binaries
	char* port_logdir; //!< logs
	*/
	
	SmallMap* make_conf;
	Vector* kernel;
};

/**
 * Returns a randomly generated HOST_ID
 * @return a new host id
 */
char* host_id_new();

/**
 * Creates a new host given an ID
 * @param server the parent server
 * @param id the ID of the Host
 * @return a pointer to the Host
 */
Host* host_new(Server* server, char* id);

/**
 * Updates the current status of the host by reading its directory
 * @param host the host to update the status of
 */
void host_set_status(Host* host);

/**
 * Writes the path of the host to dest
 * @param host the host to read path from
 * @param dest the destination of the Host's path
 */
void host_get_path (Host* host, char** dest);

/**
 * Free a host
 * @param host the host to free 
 */
void host_free(Host* host);

/**
 * Write the make.conf (also updates it)
 * @param host the host of which to write the make.conf for
 * @return 0 if successful, 1 if not successful
 */
int host_write_make_conf(Host* host);

/* Request calls */

/**
 * Safely install packages in the chroot
 * @param host the host to install packages for
 * @param arg the arguments passed to emerge
 * @return OK if successful, INTERNAL_ERROR if not
 */
response_t host_install(Host* host, char* arg);

/**
 * Initialize extras
 * @param target target host
 */
void host_init_extras(Host* target);

/**
 *
 */

#endif